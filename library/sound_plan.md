# Sound System Plan — `library/sound/`

## Design Philosophy

This sound system is **entirely custom code** — no SDL or SDL_mixer dependency. SDL and SDL_mixer were studied as reference material only, to understand audio concepts (device opening, codec architecture, mixing callbacks, channel management). The actual implementation is our own design, improving upon those patterns:

- **`IAudioDevice` + platform backends** — our own audio device interface with ALSA, WASAPI, and PulseAudio implementations. Not SDL's audio subsystem.
- **`CMixEngine`** — our own 4-bus mixer with SSE inner loop, volume hierarchy, and bus enable/disable. Not SDL_mixer's channel system.
- **Codec wrappers** — our own `ICodec` interface around stb_vorbis, minimp3, and dr_flac. Not SDL_mixer's codec dispatch. These header-only decoder libraries are public-domain and used independently of SDL_mixer.
- **`CSound` / `CPlayList` / `CSoundMgr`** — adapted from the Vulkan engine's organizational pattern but rewritten to use `CMixEngine`. No SDL_mixer API calls anywhere.

**Zero SDL or SDL_mixer headers, libraries, or function calls will exist in the final code.**

## Architecture Overview

```
╭─────────────────────────────────────────────────────────────────────────╮
│                         CSoundMgr (singleton)                          │
│  inherits CManagerBase (loadListTable / loadGroup / freeGroup)         │
│  play(group, id) / stop() / pause() / resume()                        │
│  m_soundMapMap: group → (id → CSound)                                  │
│  m_playListMapMap: group → (id → CPlayList)                            │
│  getNextChannel(bus) — round-robin per bus                             │
╰────────────────────────────┬────────────────────────────────────────────╯
                             │ owns/delegates
          ╭──────────────────┼──────────────────────╮
          ▼                  ▼                      ▼
   ╭────────────╮    ╭──────────────╮       ╭──────────────╮
   │   CSound   │    │  CPlayList   │       │  CMixEngine  │
   │ ESoundType │    │ EPlayListType│       │  (singleton) │
   │ EST_LOADED │    │ EST_RANDOM   │       │              │
   │ EST_STREAM │    │ EST_SEQUENTIAL│      │ 4 bus types: │
   │            │    │ shuffle()    │       │  Music (N)   │
   │ play/stop/ │    │ getSound()   │       │  Ambient (N) │
   │ pause/     │    │ anti-repeat  │       │  Voice (N)   │
   │ resume/vol │    ╰──────────────╯       │  SFX (N)     │
   ╰────────────╯                           ╰──────┬───────╯
                                                   │ pulls PCM
          ╭────────────────────────────────────────┤
          ▼                                        ▼
   ╭──────────────────╮                   ╭──────────────────────╮
   │  Codec Layer     │                   │  IAudioDevice        │
   │                  │                   │  (interface)         │
   │ ICodec vtable:   │                   ├──────────────────────┤
   │  WAV  (built-in) │                   │ Linux:               │
   │  OGG  (stb_vorb) │                   │  ALSAAudioDevice     │
   │  MP3  (minimp3)  │                   │  PulseAudioDevice    │
   │  FLAC (dr_flac)  │                   │ Windows:             │
   ╰──────────────────╯                   │  WASAPIAudioDevice   │
                                          ╰──────────────────────╯
```

---

## Phase 1 — Platform Audio Output + WAV Playback ✅ COMPLETE

**Goal:** Single WAV file playing through speakers on Linux and Windows.

### Files
| File | Platform | Purpose |
|------|----------|---------|
| `audiospec.h` | All | `SAudioSpec`: sample rate, channels, format, buffer size |
| `iaudiodevice.h` | All | Pure virtual: `open(spec)`, `close()`, `pause(bool)` |
| `alsaaudiodevice.h/cpp` | Linux | ALSA backend — PCM device, audio thread, pull-callback |
| `wasapiaudiodevice.h/cpp` | Windows | WASAPI shared-mode, event-driven, COM-initialized audio thread |
| `audiodevicefactory.h/cpp` | All | `CreateAudioDevice()` — `#ifdef __linux__` / `#elif defined(_WIN32)` (mirrors `windowfactory.cpp`) |
| `wavcodec.h/cpp` | All | RIFF/WAVE parser, PCM/ADPCM decode |
| `mixengine.h/cpp` | All | Singleton. Opens device via factory, audio callback. Single-channel only this phase |

### Verification
- WAV plays on Linux (ALSA) and Windows (WASAPI), no underruns or glitches

---

## Phase 2 — Multi-Format Codec Layer ✅ COMPLETE

**Goal:** Add OGG, MP3, FLAC. All header-only, fully cross-platform.

### Files
| File | Purpose |
|------|---------|
| `icodec.h` | Codec interface: `load() → PCM` (in-memory), `open/read/seek/close` (streaming) |
| `oggcodec.h/cpp` | **stb_vorbis** (bundled in `sound/stb/`) |
| `mp3codec.h/cpp` | **minimp3** (bundled in `sound/minimp3/`) |
| `flaccodec.h/cpp` | **dr_flac** (bundled in `sound/dr_libs/`) |
| `codecfactory.h/cpp` | Magic-byte format detection → appropriate `ICodec` |

### Verification
- `.ogg`, `.mp3`, `.flac`, `.wav` all play on both platforms
- Auto-detection by magic bytes works

---

## Phase 3 — Mixing Engine with 4 Bus Types ✅ COMPLETE

**Goal:** Simultaneous music + ambient + voice + SFX playback.

### Design

```
╭────────────────────────────────────────────────────────────╮
│                  CMixEngine audio callback                  │
│           (called from audio thread on all platforms)       │
│                                                            │
│  1. Zero float32 mix buffer                                │
│  2. Mix MUSIC bus (N streaming channels, decoded on-fly)   │
│  3. Mix AMBIENT bus (N channels, streaming or pre-loaded)  │
│  4. Mix VOICE bus (N channels, streaming or pre-loaded)    │
│  5. Mix SFX bus (N channels, pre-loaded PCM, round-robin)  │
│  6. Per-channel vol × per-bus vol × master vol             │
│  7. Clamp [-1.0, 1.0]                                     │
│  8. Convert F32 → device format → IAudioDevice             │
╰────────────────────────────────────────────────────────────╯
```

| Bus | Purpose | Pool |
|-----|---------|------|
| Music | Score, soundtrack, layered music | Streaming, round-robin (default 4) |
| Ambient | Environment loops — rain, wind, crowd, machinery | Separate pool (default 4) |
| Voice | Dialog, narration, announcements | Separate pool (default 4) |
| SFX | One-shot effects — clicks, explosions, footsteps | Round-robin (default 16) |

### Volume & Enable Controls

The reason for separate bus types is so the **user can independently enable/disable and control the volume of each sound category** from the game settings. The volume hierarchy is:

```
final_volume = sound_vol × bus_vol × master_vol × bus_enabled
```

- **`master_vol`** — Global volume (0.0–1.0). Affects everything.
- **`bus_vol`** — Per-bus volume (0.0–1.0). Each bus (Music, Ambient, Voice, SFX) has its own volume slider in settings.
- **`bus_enabled`** — Per-bus on/off toggle (bool). When disabled, the bus is muted (multiplied by 0). The user can disable e.g. ambient sound without affecting music or voice.
- **`sound_vol`** — Per-sound volume (0–128, set via XML `volume` attribute or at runtime via `CSound::setVolume()`). Allows individual sounds to be louder/quieter relative to their bus.

These settings are persisted via `CSettings` so they survive across sessions. Example settings XML:

```xml
<sound masterVolume="100" frequency="44100" channels="2" mixChannels="28">
    <bus name="music"   volume="80"  enabled="true"/>
    <bus name="ambient" volume="70"  enabled="true"/>
    <bus name="voice"   volume="100" enabled="true"/>
    <bus name="sfx"     volume="90"  enabled="true"/>
</sound>
```

| Concept | Detail |
|---------|--------|
| `EMixBus` | `MUSIC`, `AMBIENT`, `VOICE`, `SFX` |
| `SMixChannel` | PCM/stream, position, volume, loop count, fade, bus, paused |
| Mixing math | Float32 accumulation. `effective = sound_vol × bus_vol × master_vol × bus_enabled`. Saturating clamp `[-1.0, 1.0]` |
| Looping | `loopCount` parameter: `0` = play once, `N` = repeat N extra times, `-1` = loop indefinitely until explicitly stopped. When a sound reaches the end, if loops remain (or infinite), the play position resets to the start and the loop counter decrements (unless -1). Stopped via `CSound::stop()`, `CSoundMgr::stop()`, or `CSoundMgr::stopAllSound()` |
| Fade | `MIX_FADING_IN` / `MIX_FADING_OUT`, linear ramp per-sample |
| SSE inner loop | `_mm_mul_ps`, `_mm_add_ps`, `_mm_min_ps`, `_mm_max_ps` — works on both Linux & Windows via `<xmmintrin.h>` |
| Thread safety | Mutex on state changes; atomics for pause/volume |

Looping is especially useful for ambient sounds (rain, wind) and music that should repeat until a scene changes. Example usage:

```cpp
// Loop rain indefinitely until stopped
CSoundMgr::Instance().play("(level_1)", "rain_loop", -1);

// Play footstep 3 times total (1 play + 2 loops)
CSoundMgr::Instance().play("(level_1)", "footstep", 2);

// Stop the rain when leaving the area
CSoundMgr::Instance().stop("(level_1)", "rain_loop");
```

XML can also specify a default loop count per sound:

```xml
<load id="rain_loop" file="data/sound/ambient/rain.ogg" bus="ambient" loop="-1"/>
<load id="click" file="data/sound/sfx/click.ogg" bus="sfx" loop="0"/>
```

### Playback Controls

Full playback control at every level — individual sound, bus, and global:

| Scope | play | pause | resume | stop | setVolume | isPlaying | isPaused |
|-------|------|-------|--------|------|-----------|-----------|----------|
| **Per-sound** | `CSound::play(channel, loopCount)` | `CSound::pause()` | `CSound::resume()` | `CSound::stop()` | `CSound::setVolume(vol)` | `CSound::isPlaying()` | `CSound::isPaused()` |
| **Per-sound via manager** | `CSoundMgr::play(group, id, loop)` | `CSoundMgr::pause(group, id)` | `CSoundMgr::resume(group, id)` | `CSoundMgr::stop(group, id)` | `CSoundMgr::setVolume(group, id, vol)` | `CSoundMgr::isPlaying(group, id)` | `CSoundMgr::isPaused(group, id)` |
| **Per-bus** | — | `CMixEngine::pauseBus(bus)` | `CMixEngine::resumeBus(bus)` | `CMixEngine::stopBus(bus)` | `CMixEngine::setBusVolume(bus, vol)` | `CMixEngine::isBusPlaying(bus)` | `CMixEngine::isBusPaused(bus)` |
| **Global (all sounds)** | — | `CSoundMgr::pauseAll()` | `CSoundMgr::resumeAll()` | `CSoundMgr::stopAllSound()` | `CMixEngine::setMasterVolume(vol)` | — | — |

**`pauseAll()` / `resumeAll()`** pauses and resumes every active channel across all buses. Useful for pause menus — freeze all game audio in one call, resume exactly where it left off when unpaused. Each channel remembers its own paused state so `resumeAll()` only resumes channels that were playing before `pauseAll()` was called (channels the user had individually paused stay paused).

### Threading Model

The sound system runs on a **dedicated audio thread**, separate from the game loop. This ensures glitch-free playback regardless of frame rate or renderer load.

```
╭──────────────────────╮          ╭──────────────────────────────────────╮
│     Main Thread      │          │          Audio Thread               │
│                      │          │  (spawned by IAudioDevice::open())  │
│ CSoundMgr::play()  ──┼── mutex ─┼─▶ CMixEngine callback:             │
│ CSoundMgr::stop()  ──┼── mutex ─┼─▶   1. Read channel states         │
│ CSoundMgr::pause() ──┼── mutex ─┼─▶   2. Decode streams (if needed)  │
│ setVolume()        ──┼─ atomic ─┼─▶   3. Mix all buses into F32 buf  │
│ pauseAll()         ──┼── mutex ─┼─▶   4. Clamp + convert to device   │
│                      │          │      5. Submit to audio device       │
│ (never blocks on     │          │                                      │
│  audio — returns     │          │  Loop:                               │
│  immediately)        │          │    Wait for device ──▶ Mix ──▶ Write │
╰──────────────────────╯          ╰──────────────────────────────────────╯
```

**Per-backend loop:**

| Backend | Wait | Write |
|---------|------|-------|
| ALSA | `snd_pcm_wait()` — blocks until device needs data | `snd_pcm_writei()` |
| WASAPI | `WaitForSingleObject(hEvent)` — blocks on buffer event | `IAudioRenderClient::GetBuffer/ReleaseBuffer` |
| PulseAudio | PulseAudio write callback / `pa_threaded_mainloop` | `pa_stream_write()` |

**Synchronization rules:**
- **Mutex** protects channel state changes (play, stop, pause, resume, add/remove channels). Held briefly — game thread sets state, audio thread reads it once per buffer fill.
- **Atomics** for frequently-read values that don't need compound updates: per-bus volume, master volume, bus enabled/paused flags.
- **No locks in the hot mix loop** — channel state is snapshot-read under the mutex at the start of each callback, then the SSE mixing loop runs lock-free.
- **No heap allocation** on the audio thread. All buffers (mix buffer, work buffer, decode buffer) are pre-allocated at `init()`.
- The audio thread is set to **high priority** (`SCHED_FIFO` on Linux, `THREAD_PRIORITY_TIME_CRITICAL` on Windows) to prevent underruns.

### Verification
- Music + ambient + voice + SFX simultaneously on both platforms
- Per-bus and master volume control
- Fade in/out while other buses play
- `pauseAll()` freezes all audio, `resumeAll()` restores exactly the prior state
- Individually paused sounds remain paused after `resumeAll()`
- No audio glitches under heavy renderer load (audio thread independence)
- Stress test all channels

---

## Phase 4 — CSound, CPlayList, CSoundMgr ✅ COMPLETE

**Goal:** Manager layer matching the Vulkan engine pattern — same API, same XML format, adapted to use `CMixEngine` instead of SDL_mixer.

### Files (direct analogs from the Vulkan engine)

| File | Mirrors | Key changes from Vulkan version |
|------|---------|--------------------------------|
| `sound.h/cpp` | `SDL3-Vulkan-Game-Engine/library/sound/sound.h/cpp` | Replace `Mix_Chunk*`/`Mix_Music*` with our PCM buffer / stream handle. Replace `Mix_PlayChannel`/`Mix_PlayMusic` with `CMixEngine::play()`. Replace `MIX_MAX_VOLUME` with our own constant. Add `EMixBus m_bus` member so each sound knows its bus. Remove SDL_mixer dependency entirely |
| `playlist.h/cpp` | `SDL3-Vulkan-Game-Engine/library/sound/playlist.h/cpp` | Identical structure — `EST_RANDOM`/`EST_SEQUENTIAL`, `shuffle()` with anti-repeat, `getSound()`. Delegates to adapted `CSound`. Replace `<common/defs.h>` with our `<sound/sounddefs.h>` |
| `soundmanager.h/cpp` | `SDL3-Vulkan-Game-Engine/library/sound/soundmanager.h/cpp` | Same `CManagerBase` inheritance, same `m_soundMapMap`/`m_playListMapMap`, same playlist-first lookup in `getSound()`, same `loadGroup()`/`freeGroup()`. Replace `Mix_Init`/`Mix_OpenAudio`/`Mix_CloseAudio` with `CMixEngine::init()`/`shutdown()`. Replace `Mix_PlayingMusic`/`Mix_HaltMusic` stream helpers with `CMixEngine` equivalents. Add bus-aware channel allocation (`getNextChannel(bus)`) |
| `sounddefs.h` | New | `EMixBus`, `ESoundType`, `EPlayListType`, volume constants, channel pool defaults |

### XML Format (matches Vulkan engine, adds `bus` attribute)

```xml
<!-- soundListTable.lst -->
<listTable>
  <groupList groupName="(menu)">
    <file path="data/sound/menuSound.lst"/>
  </groupList>
</listTable>

<!-- menuSound.lst -->
<soundList>
  <soundFiles>
    <load id="click" file="data/sound/sfx/click.ogg" bus="sfx"/>
    <load id="voice_intro" file="data/sound/voice/intro.ogg" bus="voice"/>
    <load id="rain_loop" file="data/sound/ambient/rain.ogg" bus="ambient"/>
    <stream id="cave_drip" file="data/sound/ambient/cave_drip.ogg" bus="ambient"/>
    <stream id="theme" file="data/sound/music/theme.ogg" bus="music"/>
  </soundFiles>
  <playlistSet>
    <playList id="highlights" playtype="random">
      <sound id="click"/>
    </playList>
  </playlistSet>
</soundList>
```

### Verification
- Load/free groups, play by sound ID and playlist ID
- `play("(menu)", "highlights")` checks playlist first, returns random sound (same behavior as Vulkan engine)
- Voice + music + ambient + SFX concurrently via bus routing
- Works identically on Linux and Windows

---

## Phase 5 — Integration & Polish ✅ COMPLETE

| Task | Detail | Status |
|------|--------|--------|
| `CGame` integration | `CSoundMgr::Instance().init()` in `CGame` constructor, `cleanup()` in destructor | ✅ Done |
| `CStartUpState` loading | `LoadListTable` and `loadGroup` in `CStartUpState::Load()` | ✅ Done |
| Sound data directory | `data/sound/` with `soundListTable.lst` and `menuSound.lst` | ✅ Done |
| CMake link flags | Linux: `-lasound -lpthread` via pkg-config ALSA. Windows: `ole32.lib mmdevapi.lib` via pragma | ✅ Done |
| PulseAudio backend (Linux) | `PulseAudioDevice` — future optional improvement | Not yet |
| Sample rate conversion | Linear interpolation resampler in `NAudioResample::resample()`. Called automatically by `CSound::loadFromNode()` after loading. | ✅ Done |

---

## Header-Only Libraries to Bundle

| Library | Format | License | Platform |
|---------|--------|---------|----------|
| [stb_vorbis](https://github.com/nothings/stb) | OGG Vorbis | Public domain | All |
| [minimp3](https://github.com/lieff/minimp3) | MP3 | CC0 | All |
| [dr_flac](https://github.com/mackron/dr_libs) | FLAC | Public domain | All |

---

## Final File Tree

```
library/sound/
├── sounddefs.h                   # Enums, constants, bus config
├── audiospec.h                   # SAudioSpec struct
├── iaudiodevice.h                # Platform audio interface
├── alsaaudiodevice.h/cpp         # ALSA backend (Linux)
├── pulseaudiodevice.h/cpp        # PulseAudio backend (Linux, Phase 5)
├── wasapiaudiodevice.h/cpp       # WASAPI backend (Windows)
├── audiodevicefactory.h/cpp      # #ifdef platform selection
├── icodec.h                      # Codec interface
├── wavcodec.h/cpp                # WAV decoder
├── oggcodec.h/cpp                # OGG via stb_vorbis
├── mp3codec.h/cpp                # MP3 via minimp3
├── flaccodec.h/cpp               # FLAC via dr_flac
├── codecfactory.h/cpp            # Magic-byte format detection
├── mixengine.h/cpp               # 4-bus mixer with SSE inner loop
├── sound.h/cpp                   # CSound (mirrors Vulkan engine)
├── playlist.h/cpp                # CPlayList (mirrors Vulkan engine)
├── soundmanager.h/cpp            # CSoundMgr (mirrors Vulkan engine)
├── stb/stb_vorbis.c              # Bundled
├── minimp3/minimp3.h             # Bundled
├── minimp3/minimp3_ex.h          # Bundled
└── dr_libs/dr_flac.h             # Bundled
```

## CMakeLists.txt Additions

```cmake
# Cross-platform sound files
list(APPEND LIBRARY_SOURCES
    sound/mixengine.cpp
    sound/wavcodec.cpp
    sound/oggcodec.cpp
    sound/mp3codec.cpp
    sound/flaccodec.cpp
    sound/codecfactory.cpp
    sound/audiodevicefactory.cpp
    sound/sound.cpp
    sound/playlist.cpp
    sound/soundmanager.cpp
    sound/stb/stb_vorbis.c )

if(UNIX)
    list(APPEND LIBRARY_SOURCES
        sound/alsaaudiodevice.cpp
        sound/pulseaudiodevice.cpp )
endif()

if(WIN32)
    list(APPEND LIBRARY_SOURCES
        sound/wasapiaudiodevice.cpp )
endif()

include_directories( ... sound )
```

Platform-specific code is isolated to 3 backend files + the factory — everything else (including `sound.h`, `playlist.h`, `soundmanager.h`) is shared, exactly like the Vulkan engine originals.
