
/************************************************************************
*    FILE NAME:       wasapiaudiodevice.cpp
*
*    DESCRIPTION:     WASAPI audio device implementation for Windows
************************************************************************/

#ifdef _WIN32

// Physical component dependency
#include <sound/wasapiaudiodevice.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Windows lib dependencies
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>

// Standard lib dependencies
#include <cstring>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "avrt.lib")


/************************************************************************
*    DESC:  Constructor
************************************************************************/
CWasapiAudioDevice::CWasapiAudioDevice() :
    m_pAudioClient( nullptr ),
    m_pRenderClient( nullptr ),
    m_hEvent( nullptr ),
    m_hTask( nullptr ),
    m_bufferFrameCount( 0 ),
    m_callback( nullptr ),
    m_pUserData( nullptr ),
    m_running( false ),
    m_paused( false ),
    m_comInitialized( false )
{
}


/************************************************************************
*    DESC:  Destructor
************************************************************************/
CWasapiAudioDevice::~CWasapiAudioDevice()
{
    close();
}


/************************************************************************
*    DESC:  Open the WASAPI device in shared mode
************************************************************************/
void CWasapiAudioDevice::open( const SAudioSpec & spec, AudioCallback callback, void * pUserData )
{
    m_spec = spec;
    m_callback = callback;
    m_pUserData = pUserData;

    // Initialize COM for this thread (main thread)
    HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    if( SUCCEEDED(hr) )
        m_comInitialized = true;
    else if( hr != RPC_E_CHANGED_MODE )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to initialize COM.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Get default audio endpoint
    IMMDeviceEnumerator * pEnumerator = nullptr;
    hr = CoCreateInstance( __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&pEnumerator) );
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to create device enumerator.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    IMMDevice * pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint( eRender, eConsole, &pDevice );
    pEnumerator->Release();
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to get default audio endpoint.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Activate audio client
    hr = pDevice->Activate( __uuidof(IAudioClient), CLSCTX_ALL, nullptr,
        reinterpret_cast<void**>(&m_pAudioClient) );
    pDevice->Release();
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to activate audio client.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Set up wave format — F32 interleaved
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag     = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels      = m_spec.channels;
    wfx.nSamplesPerSec = m_spec.sampleRate;
    wfx.wBitsPerSample = 32;
    wfx.nBlockAlign    = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    // Create event for buffer notification
    m_hEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
    if( !m_hEvent )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to create audio event.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Initialize audio client in shared mode with auto-convert
    DWORD flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK
                | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
                | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;

    // Buffer duration in 100-nanosecond units
    REFERENCE_TIME duration = static_cast<REFERENCE_TIME>(
        10000000ULL * m_spec.bufferFrames / m_spec.sampleRate );

    hr = m_pAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, flags,
        duration, 0, &wfx, nullptr );
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to initialize audio client (0x%08X).\n\n%s\nLine: %d",
                static_cast<unsigned>(hr), __FUNCTION__, __LINE__ ) );

    // Set event handle for buffer notifications
    hr = m_pAudioClient->SetEventHandle( m_hEvent );
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to set event handle.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Query actual buffer size
    hr = m_pAudioClient->GetBufferSize( &m_bufferFrameCount );
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to get buffer size.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Get render client service
    hr = m_pAudioClient->GetService( __uuidof(IAudioRenderClient),
        reinterpret_cast<void**>(&m_pRenderClient) );
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to get render client.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    // Pre-allocate callback buffer
    m_callbackBuffer.resize( m_bufferFrameCount * m_spec.channels );

    // Start playback and audio thread
    m_running = true;
    m_paused = false;

    hr = m_pAudioClient->Start();
    if( FAILED(hr) )
        throw NExcept::CCriticalException( "WASAPI Audio Error!",
            NGenFunc::FormatString( "Failed to start audio client.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );

    m_thread = std::thread( &CWasapiAudioDevice::audioThread, this );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "WASAPI audio device opened: %d Hz, %d ch, %d frames buffer",
        m_spec.sampleRate, m_spec.channels, m_bufferFrameCount ) );
}


/************************************************************************
*    DESC:  Stop the audio thread and release all COM objects
************************************************************************/
void CWasapiAudioDevice::close()
{
    if( m_running )
    {
        m_running = false;

        if( m_thread.joinable() )
            m_thread.join();
    }

    if( m_pAudioClient )
        m_pAudioClient->Stop();

    if( m_pRenderClient )
    {
        m_pRenderClient->Release();
        m_pRenderClient = nullptr;
    }

    if( m_pAudioClient )
    {
        m_pAudioClient->Release();
        m_pAudioClient = nullptr;
    }

    if( m_hEvent )
    {
        CloseHandle( m_hEvent );
        m_hEvent = nullptr;
    }

    m_callbackBuffer.clear();

    if( m_comInitialized )
    {
        CoUninitialize();
        m_comInitialized = false;
    }
}


/************************************************************************
*    DESC:  Pause or resume audio output
************************************************************************/
void CWasapiAudioDevice::pause( bool paused )
{
    m_paused = paused;
}


/************************************************************************
*    DESC:  Audio thread — event-driven buffer filling
************************************************************************/
void CWasapiAudioDevice::audioThread()
{
    // COM must be initialized per-thread
    CoInitializeEx( nullptr, COINIT_MULTITHREADED );

    // Set thread priority for low-latency audio via MMCSS
    DWORD taskIndex = 0;
    m_hTask = AvSetMmThreadCharacteristicsW( L"Pro Audio", &taskIndex );
    if( !m_hTask )
        NGenFunc::PostDebugMsg( "WASAPI: Could not set MMCSS thread priority" );

    while( m_running )
    {
        // Wait for buffer event (200ms timeout for clean shutdown)
        DWORD waitResult = WaitForSingleObject( m_hEvent, 200 );
        if( waitResult != WAIT_OBJECT_0 )
            continue;

        // Query available buffer space
        UINT32 padding = 0;
        HRESULT hr = m_pAudioClient->GetCurrentPadding( &padding );
        if( FAILED(hr) )
            continue;

        UINT32 available = m_bufferFrameCount - padding;
        if( available == 0 )
            continue;

        // Get endpoint buffer
        BYTE * pData = nullptr;
        hr = m_pRenderClient->GetBuffer( available, &pData );
        if( FAILED(hr) )
            continue;

        if( m_paused )
        {
            // Write silence via flag (no memset needed)
            m_pRenderClient->ReleaseBuffer( available, AUDCLNT_BUFFERFLAGS_SILENT );
        }
        else
        {
            // Zero and fill via mix callback
            const uint32_t totalSamples = available * m_spec.channels;
            std::memset( m_callbackBuffer.data(), 0, totalSamples * sizeof(float) );

            if( m_callback )
                m_callback( m_pUserData, m_callbackBuffer.data(), available );

            // Copy F32 data to endpoint buffer
            std::memcpy( pData, m_callbackBuffer.data(), totalSamples * sizeof(float) );

            m_pRenderClient->ReleaseBuffer( available, 0 );
        }
    }

    // Cleanup thread resources
    if( m_hTask )
    {
        AvRevertMmThreadCharacteristics( m_hTask );
        m_hTask = nullptr;
    }

    CoUninitialize();
}

#endif // _WIN32
