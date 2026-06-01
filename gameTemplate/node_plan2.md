# CNode System — Redesigned Merge Plan (v2)

## Analysis of the Vulkan/WebGL Engine Node System

Both the Vulkan engine (`SDL2-Vulkan-Game-Engine/library/node/`) and the JavaScript/WebGL engine (`javajcript-webgl-game-engine/library/node/`) share the same problematic architecture: a deep class hierarchy with specialized node subclasses.

### Current Vulkan Engine Hierarchy (8 concrete classes)

```diagram
                          ╭────────╮
                          │ iNode  │  ← interface + leaf base
                          ╰───┬────╯
              ┌───────────────┼───────────────────────┐
              │               │                       │
     ╭────────┴───────╮  ╭───┴──────────────╮  ╭─────┴──────────────╮
     │     CNode      │  │ CSpriteLeafNode  │  │ CUIControlLeafNode │
     ╰────────┬───────╯  ╰──────────────────╯  ╰────────────────────╯
              │
     ╭────────┴────────╮
     │   CRenderNode   │
     ╰────────┬────────╯
     ┌────────┼──────────────────┐
     │        │                  │
╭────┴─────╮ ╭┴──────────╮  ╭───┴──────────╮
│CObjectNode│ │CSpriteNode│  │CUIControlNode│
╰──────────╯ ╰───────────╯  ╰──────────────╯
```

### Problems Identified

1. **Class explosion:** 8 classes for 3 payload types. Adding a new type (particle emitter, audio source) requires 2+ new classes (leaf + branch) and touching the factory. Both the Vulkan and WebGL engines have this exact duplication.

2. **Multiple inheritance:** `CSpriteNode` inherits both `CRenderNode` and `CSprite`. `CObjectNode` inherits both `CRenderNode` and `CObject`. Comments in the Vulkan code even acknowledge this: *"Make use of multiple inheritance so that the sprite can return a pointer to the node without having to keep a pointer to it."* This is a workaround for a design that shouldn't need the workaround.

3. **Duplicated recursion (worst offender):** `CRenderNode` has 4 nearly identical recursive methods (`update`, `transform`, `recordCommandBuffer`, `destroyPhysics`). Each uses the same pattern:
   ```
   get iterator → do/while loop → get next → type-switch → call method → recurse
   ```
   Every subclass then overrides these to add "do my thing, then call CRenderNode::method()." The recursion template is copy-pasted 8+ times across the codebase.

4. **Leaf/branch duplication:** `CSpriteLeafNode` and `CSpriteNode` duplicate all sprite-specific logic (constructor, update, transform, render, getSprite, getObject, getRadius, getSize). The only difference: one has children, one doesn't. An empty `std::vector` costs 24 bytes — that's the "overhead" the leaf nodes were created to avoid.

5. **Node IS the payload:** Because of multiple inheritance, `CSpriteNode` *is* the sprite. You can't reuse a sprite across nodes, swap payloads at runtime, or test the tree independently.

6. **Awkward iteration API:** The `getNodeIter()` / `next(iter)` pattern exposes raw iterators through virtual calls. This is error-prone (caller manages iterator state) and blocks range-based `for` loops.

7. **Expensive parent lookup:** `findParent()` is O(n) — it walks the entire tree to find a node's parent. A simple parent pointer makes this O(1).

8. **Dummy globals:** `inode.cpp` declares global `float dummyRadius` and `CSize<float> dummySize` returned by default from virtual methods. These are unnecessary — returning by value with default values is cleaner.

9. **Bug in `CObjectNode`:** The CRC16 calculation is duplicated (lines 26-31 of `objectnode.cpp` compute `m_crcUserId` twice).

### What Both Engines Got Right (keep these)

- Atomic handle generation (`std::atomic<handle16_t>`) for thread-safe unique node IDs
- Name-based child lookup for node resolution
- XML-driven tree construction via `CNodeDataList` (recursive parse → flat vector with parent/child IDs)
- Factory pattern for node creation from data
- `init()` called after full tree assembly for accumulated size/radius computation
- UI_CONTROL node type for in-game UI controls attached to sprites (health bars, boost meters, etc.)

---

## Proposed Design: Single Composition-Based CNode

### Two-Layer Data Architecture

The engine separates data into two layers for reusability:

```diagram
╭──────────────────────────────────────────────────────╮
│  Strategy / Node Layer                               │
│  Position, rotation, scale, scripts, tree structure  │
│  "How and where to use it"                           │
╰──────────────┬───────────────────────────────────────╯
               │ references (by group + name)
╭──────────────┴───────────────────────────────────────╮
│  Object Data Layer (CObjectDataMgr)                  │
│  Texture, mesh, color, shader, vertex data           │
│  "What it is"                                        │
╰──────────────────────────────────────────────────────╯
```

The same object data can be reused across multiple node instances with different positions, rotations, scales, and scripts. CNode is the building block of the strategy system — strategies compose nodes into higher-level constructs (game entities, UI screens, levels). The menu system will also be restructured to use CNode so that menus and gameplay strategies share the same infrastructure.

### Real-World Example: Player Ship Strategy

From `javajcript-webgl-game-engine/invaders/data/objects/strategy/level1/playerShip.strategy` — a single strategy file defines a complex multi-sprite game entity using the node system:

```diagram
strategy (defaultGroup="(level_1)", camera="levelCamera")
│
├── object root (z=-70)              ← positions the whole ship in the world
│
├── node "player_shot"               ← sprite: laser spawn point, with shoot script
├── node "explode"                   ← sprite: explosion animation
╰── node "player_ship"              ← sprite: null_player_ship (2x scale, die/hit scripts)
    │
    ├── node "UIBoostBar"            ← uiProgressBar (boost meter, child of ship)
    ├── node "UIProgressBar"         ← uiProgressBar (health bar, child of ship)
    ╰── node "playerShip_object"     ← object (grouping node)
        │
        ├── node "player_fire_tail"  ← sprite: engine flame (hidden, animated by script)
        ╰── node "player_ship_body"  ← sprite: the actual ship visual
```

This demonstrates:
- **Multiple node types in one tree** — sprites, objects, and UI controls all as children of the same node.
- **Transform hierarchy** — the ship body, fire tail, boost bar, and health bar all move with the parent ship sprite.
- **Reusability** — `defaultGroup="(level_1)"` references shared object data; the strategy defines how the sprite needs to render: position, rotation, scale, scripts, etc.
- **UI controls as node children** — progress bars attached to the ship node, confirming why `UI_CONTROL` belongs in `ENodeType`.

**Core principle:** One `CNode` class. The tree structure (parent, children, recursion) is handled by CNode. Domain-specific data (sprite, object, in-game UI control) is attached via `std::variant` composition. No subclasses. No multiple inheritance. The GUI menu system remains separate — menus are more straightforward and don't need scene graph infrastructure.

```diagram
╭──────────────────────────────────────────────────────╮
│                       CNode                          │
│                                                      │
│  handle16_t            m_handle     (atomic, unique)  │
│  int16_t               m_userId                      │
│  std::string           m_name       (node name)      │
│  uint8_t               m_nodeId                      │
│  uint8_t               m_parentId   (build-time ID)  │
│  bool                  m_headNode                    │
│  ENodeType             m_type                        │
│                                                      │
│  CNode*                m_parent     (O(1) parent)    │
│  std::vector<std::unique_ptr<CNode>> m_children      │
│                                                      │
│  NodePayload_t         m_payload                     │
│    = std::variant<                                   │
│        CObject,                   ← group/transform  │
│        std::unique_ptr<CSprite>,  ← visual sprite    │
│        std::unique_ptr<CUIControl>← in-game UI ctrl  │
│      >                                               │
│                                                      │
│  update()         → payload dispatch + recurse       │
│  transform()      → payload dispatch + recurse       │
│  render(camera)   → payload dispatch + recurse       │
│  getObject()      → CObject* from any alternative    │
│  getSprite()      → CSprite* or nullptr              │
│  getControl()     → CUIControl* or nullptr           │
│  findChild(name)  → recursive string name search      │
│  addNode(node)    → find parent by ID, attach child  │
│  init()           → mark head, compute size/radius   │
╰──────────────────────────────────────────────────────╯
```

### Why `std::variant`, not `std::unique_ptr<iPayload>`

| Criteria | `std::variant` | `virtual iPayload*` |
|---|---|---|
| Heap allocations | 0 (payload inline) | 1 per node |
| Virtual dispatch | None (`std::visit` compiles to jump table) | vtable + indirect call |
| Cache locality | Payload data inline in node | Pointer chase to heap |
| Adding new types | Modify variant typedef, recompile | Add new subclass, recompile anyway |
| Code complexity | `std::visit` with overloaded lambdas | Interface + N implementations |

For a game engine where execution speed matters and the set of payload types is small and stable, `std::variant` is the correct choice.

### Why `CObject` instead of `std::monostate`

Every node in a scene graph needs transform capability (position, rotation, scale) for parent→child transform propagation. A `std::monostate` grouping node would have no transform data, making it useless as a scene graph parent. Using `CObject` as the default variant alternative means every node can participate in transform hierarchies.

`CSprite` and `CUIControl` both inherit from `CObject`, so `getObject()` works uniformly for all three alternatives — it returns the `CObject*` whether the node holds a bare `CObject`, a `CSprite`, or a `CUIControl`.

### Why no leaf/branch split

An empty `std::vector<std::unique_ptr<CNode>>` costs 24 bytes (3 pointers: begin, end, capacity) and zero heap allocation. The "overhead" that `CSpriteLeafNode` was designed to avoid is negligible. Eliminating the leaf/branch split halves the number of classes and removes all duplicate logic.

---

## Phase 1 — Foundation: `nodedefs.h`, `CNodeData`, `CNodeDataList`

**Goal:** Create `../library/node/` with data structures for XML parsing. No runtime node class yet.

### Files to create

- **`../library/node/nodedefs.h`**
  - `ENodeType` enum: `{ _NULL_ = 0, OBJECT, SPRITE, UI_CONTROL }` (for in-game UI controls attached to sprites, e.g., health bars)
  - `EControlType` enum: `{ _NULL_ = 0, METER, PROGRESS_BAR }` (UI control subtypes)
  - `handle16_t` typedef (`uint16_t`)
  - Constants: `defs_DEFAULT_NODE_ID = 0`, `defs_DEFAULT_ID = -1`, `defs_DEFAULT_HANDLE = 0`

- **`../library/node/nodedata.h` / `nodedata.cpp`**
  - Port from Vulkan engine's `CNodeData`, but **do not inherit from `CSpriteData`** (this engine doesn't have `CSpriteData`).
  - Stores: `m_nodeName`, `m_nodeId`, `m_parentNodeId`, `m_userId`, `m_nodeType`, `m_hasChildrenNodes`, `m_group`, `m_objectName`, plus a reference/copy of the `XMLNode` for deferred loading.
  - Node type detection: look for `<sprite>`, `<object>`, or UI control child elements (`<uiMeter>`, `<uiProgressBar>`) in XML.

- **`../library/node/nodedatalist.h` / `nodedatalist.cpp`**
  - Port from Vulkan engine's `CNodeDataList`.
  - Recursive XML tree → flat `std::vector<CNodeData>` with parent/child IDs via `m_idCounter`.
  - Two constructors: XML-based (full tree parse) and dynamic (group + objectName for programmatic sprite creation).

### Modifications

- **`../library/CMakeLists.txt`** — Add `node/nodedata.cpp` and `node/nodedatalist.cpp` to `LIBRARY_SOURCES`. Add `node` to `include_directories`.
- **`../library/common/defs.h`** — No changes needed (node-specific defs go in `nodedefs.h` to keep defs.h clean).

### Validation

- Library compiles with the new files.
- Construct a `CNodeDataList` from a test XML string. Verify the flat vector has correct parent/child IDs and node types.

**⏸ STOP — Review Phase 1 before proceeding.**

---

## Phase 2 — CNode Class and Payload System

**Goal:** Implement the single `CNode` class with `std::variant` payload and recursive tree operations.

### Files to create

- **`../library/node/node.h` / `node.cpp`**

  **Data members:**
  ```cpp
  using NodePayload_t = std::variant<CObject, std::unique_ptr<CSprite>, std::unique_ptr<CUIControl>>;

  handle16_t   m_handle;       // Unique, from atomic counter
  int16_t      m_userId;       // User-assigned ID
  std::string  m_name;         // Node name (empty = unnamed)
  uint8_t      m_nodeId;       // Build-time ID for tree assembly
  uint8_t      m_parentId;     // Build-time parent ID
  bool         m_headNode;     // True if this is the root
  ENodeType    m_type;         // OBJECT or SPRITE

  CNode*                              m_parent;    // O(1) parent access
  std::vector<std::unique_ptr<CNode>> m_children;  // RAII ownership
  NodePayload_t                       m_payload;   // Composition
  float                               m_radius;    // Accumulated bounding radius
  CSize<float>                        m_size;      // Accumulated bounding size
  ```

  **Key methods:**
  - `CNode(uint8_t nodeId, uint8_t parentId)` — default constructor, `CObject` payload (group node).
  - `CNode(const CNodeData&)` — construct with appropriate payload based on node type.
  - `~CNode()` — default (unique_ptr handles child deletion).
  - `addNode(std::unique_ptr<CNode>)` — find parent by `m_nodeId` match, attach child, set `m_parent` pointer.
  - `pushBackNode(std::unique_ptr<CNode>)` — direct child append.
  - `findParent(uint8_t parentId)` → `CNode*` — recursive search by build-time ID.
  - `findChild(const std::string& name)` → `CNode*` — recursive search by string name comparison.
  - `init()` — mark as head node, compute accumulated size/radius by walking children.
  - `getObject()` → `CObject*` — returns CObject from any variant alternative (CSprite and CUIControl both inherit CObject).
  - `getSprite()` → `CSprite*` — returns CSprite if payload is sprite, else nullptr.
  - `getControl()` → `CUIControl*` — returns CUIControl if payload is UI control, else nullptr.
  - `getParent()` → `CNode*` — O(1) via stored pointer.
  - `getHandle()`, `getId()`, `getNodeId()`, `getParentId()`, `getType()` — simple accessors.
  - `getRadius()`, `getSize()` — return accumulated values for head nodes, per-node values otherwise.

  **Recursive game loop methods (no subclasses needed):**
  ```cpp
  void update() {
      // Dispatch to payload
      std::visit([](auto& p) {
          using T = std::decay_t<decltype(p)>;
          if constexpr (std::is_same_v<T, std::unique_ptr<CSprite>>)
              p->Update();
          else if constexpr (std::is_same_v<T, std::unique_ptr<CUIControl>>)
              p->update();
          // CObject has no update — intentional no-op
      }, m_payload);

      // Recurse children
      for (auto& child : m_children)
          child->update();
  }

  void transform() {
      getObject()->transform();
      for (auto& child : m_children)
          child->transform(*getObject());  // parent propagation
  }

  void transform(const CObject& parent) {
      getObject()->transform(parent);
      for (auto& child : m_children)
          child->transform(*getObject());
  }

  void render(const CCamera& camera) {
      std::visit([&camera](auto& p) {
          using T = std::decay_t<decltype(p)>;
          if constexpr (std::is_same_v<T, std::unique_ptr<CSprite>>)
              p->render(camera);
          else if constexpr (std::is_same_v<T, std::unique_ptr<CUIControl>>)
              p->render(camera);
      }, m_payload);

      for (auto& child : m_children)
          child->render(camera);
  }
  ```

  **Key design difference from Vulkan engine:** All recursion is in ONE place (CNode), using range-based `for` over `m_children`. No virtual dispatch, no iterator gymnastics, no type-switching in the recursion. The `std::visit` compiles to a branch on the variant discriminator — equivalent to one `if` check, which the branch predictor will handle well since node types are spatially coherent.

### Modifications

- **`../library/CMakeLists.txt`** — Add `node/node.cpp`.

### Validation

- Construct a CNode with default payload (group/object node). Verify `getObject()` returns non-null, `getSprite()` returns nullptr.
- Construct a CNode with sprite payload. Verify `getSprite()` returns non-null.
- Build a 3-level tree: object root → two sprite children. Call `transform()` on root, verify parent transforms propagate.
- Call `update()` and `render()` on the tree, verify no crashes.

**⏸ STOP — Review Phase 2 before proceeding.**

---

## Phase 3 — Node Factory

**Goal:** Implement the factory that builds node trees from `CNodeDataList`.

### Files to create

- **`../library/node/nodefactory.h` / `nodefactory.cpp`**
  - `namespace NNodeFactory`
  - `std::unique_ptr<CNode> Create(const CNodeData&)` — creates a single node:
    - `SPRITE` → CNode with `std::unique_ptr<CSprite>` payload, constructed from `CObjectDataMgr::Instance().getData(group, objectName)`. Loads additional XML data (transforms, etc.) from the node data's XMLNode.
    - `OBJECT` → CNode with `CObject` payload. Loads transforms from XMLNode via `CObject::loadTransFromNode()`.
    - `UI_CONTROL` → CNode with `std::unique_ptr<CUIControl>` payload, constructed based on `CNodeData::getControlType()` (meter, progress bar). For in-game UI controls attached to sprites (e.g., health bars on ships).
    - `_NULL_` → CNode with default `CObject` payload (identity transform).
  - `std::unique_ptr<CNode> Create(const CNodeDataList&)` — builds the full tree:
    1. Create all nodes from the data list.
    2. First node is the root. Remaining nodes find their parent via `addNode()`.
    3. Call `init()` on the root.
    4. Return the root.

### Modifications

- **`../library/CMakeLists.txt`** — Add `node/nodefactory.cpp`.

### Validation

- Create a test XML node definition with nested nodes (object root, sprite children).
- Parse with `CNodeDataList`, build with `NNodeFactory::Create()`.
- Verify: tree structure, node types, parent-child relationships, string name lookups.
- Full library compile and link.

**⏸ STOP — Review Phase 3 before proceeding.**

---

## Phase 4 — Game Integration — SKIPPED

**Skipped:** The node system will be wired into the game template through the strategy system when that is implemented. No standalone `CNodeMgr` or direct game state integration needed — the strategy owns the nodes.

---

## Phase 5 — Polish and Hardening — COMPLETE

**Goal:** Finalize memory management, edge cases, and documentation.

### Results

- ✅ **RAII verified:** `std::vector<std::unique_ptr<CNode>>` children — default destructor handles cleanup. No manual delete anywhere.
- ✅ **Accumulated size/radius:** `init()` walks children via `calcSize()`, computes bounding extents. Dead code (empty if/pVisual block) removed.
- ✅ **Name lookup:** `findChild()` uses recursive `std::string` comparison. Works for any tree depth.
- ✅ **Thread safety:** Handle generation is `std::atomic<handle16_t>`. No other shared mutable state — each node tree is owned by a single strategy.
- ✅ **Header documentation:** `node.h` has a comment block documenting the composition-based design, payload types, and variant dispatch.
- ✅ **No temporary test code** — none was created.
- ✅ **Debug + Release builds** succeed with zero warnings on node/gui files.
- ✅ **`gui/uicontrol.h/.cpp`** stub created for CUIControl payload support (inherits CObject, has update/transform/render hooks).

---

## Open Questions

1. **~~CRC16 implementation~~ — RESOLVED:** Use option (c) — store full `std::string` names on nodes and compare directly. Zero collision risk, names are typically short so the performance difference is negligible. This eliminates the need for a CRC16 function entirely.

2. **~~`CNodeData` and `CSpriteData`~~ — RESOLVED:** No `CSpriteData` class needed. `CNodeData` stores group, objectName, and XMLNode directly — the XMLNode already carries all the data that `CSpriteData` would have held.

3. **~~Node data directory~~ — RESOLVED:** Node XML definitions will live under `data/objects/strategy/` matching the Vulkan and JavaScript/WebGL engines' pattern. The strategy system will be implemented in the future and node trees are part of that system. This keeps all object-related data under one tree with consistent `LoadListTable`/`loadGroup` flow.

4. **~~`CSprite` construction in payload~~ — RESOLVED:** `CObjectDataMgr` keeps the object data alive for the lifetime of the application. The node/payload does not need to own a copy — storing a `const iObjectData&` reference (current `CSprite` pattern) is correct.

5. **~~Light and camera propagation~~ — RESOLVED:** Light propagation and camera propagation will both be handled by the strategy system, not the node system. CNode does not need light or camera-related methods — the strategy will call `setLights()` and pass the appropriate camera(s) to individual sprites as needed.

6. **~~Physics support~~ — RESOLVED:** No physics hooks in the node system for now. If 2D physics are added later, it will follow the same Box2D integration pattern as the other engines and will be wired in at that time.

7. **~~Dynamic node manipulation~~ — RESOLVED:** Keep it simple for now. Reparenting, removing children, and payload swapping can be added when needed.

8. **~~Render order~~ — RESOLVED:** Children render in insertion order, matching the Vulkan engine's behavior.
