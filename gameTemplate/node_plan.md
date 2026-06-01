# CNode System — Merge & Redesign Plan

## Problem Statement

The Vulkan engine's node system has **8 concrete node classes** in a deep inheritance tree:

```diagram
                          ╭────────╮
                          │ iNode  │
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

**Why this is bad design:**

1. **Class explosion** — Each payload type × leaf/branch = 2 classes, plus CRenderNode as glue. Adding a new payload type (e.g., particle emitter, audio source) requires 2+ new classes and touching the factory.
2. **Multiple inheritance** — `CSpriteNode` inherits both `CRenderNode` and `CSprite`. `CObjectNode` inherits both `CRenderNode` and `CObject`. This creates tight coupling and diamond-shaped dependencies.
3. **Duplicated recursion** — Every `CRenderNode` subclass overrides `update()`/`transform()`/`recordCommandBuffer()` with the same pattern: "do my thing, then recurse children." The recursion logic is copy-pasted.
4. **Leaf vs. Branch duplication** — `CSpriteLeafNode` and `CSpriteNode` duplicate all sprite-specific logic; the only difference is whether they have a child vector.
5. **The node IS the payload** — Because of multiple inheritance, the node *is* the sprite/object. This means you can't reuse a sprite across nodes, swap payloads at runtime, or test nodes without full sprite construction.

---

## Proposed Design: Composition-Based CNode

**Core idea:** One `CNode` class that owns the tree structure. Payloads are attached via composition using `std::variant` (or `std::unique_ptr<iNodePayload>`). The node handles recursion; payloads handle domain logic.

```diagram
╭───────────────────────────────────────────────╮
│                    CNode                      │
│                                               │
│  handle, userId, crcUserId, nodeId, parentId  │
│  std::vector<CNode*>  m_children              │
│  ENodeType             m_type                 │
│  std::variant<                                │
│    std::monostate,          ← empty/group     │
│    CSpritePayload,          ← owns CSprite    │
│    CObjectPayload           ← owns CObject    │
│  >                    m_payload               │
│                                               │
│  update() / transform() / render()            │
│  → dispatch to payload, then recurse children │
╰───────────────────────────────────────────────╯

╭─────────────────╮  ╭──────────────────╮
│ CSpritePayload  │  │ CObjectPayload   │
│                 │  │                  │
│ CSprite m_sprite│  │ CObject m_object │
│ update()        │  │ transform()      │
│ transform()     │  │                  │
│ render()        │  │                  │
╰─────────────────╯  ╰──────────────────╯
```

**Benefits:**
- Adding a new node type = add one payload struct + one variant alternative. No new node classes.
- No multiple inheritance. No duplication. One recursion implementation.
- Leaf vs. branch is just whether `m_children` is empty — no separate classes needed.
- Payloads can be swapped, nodes can be tested independently.
- CRC16 lookup, atomic handles, parent/child IDs all remain.

---

## Phase 1 — Scaffold: `CNode`, `iNode`, and `CNodeData` (Foundation)

**Goal:** Create the node folder in the library with the core data structures, no payload logic yet.

### Files to create:
- `../library/node/inode.h` / `inode.cpp` — Minimal interface. Handle generation (atomic `handle16_t`), `m_userId`, `m_crcUserId`, `m_nodeId`, `m_parentId`, `m_headNode`, `m_type` enum. Virtual destructor. No tree methods — those move to CNode.
- `../library/node/node.h` / `node.cpp` — `CNode` extends `iNode`. Owns `std::vector<CNode*> m_children`. Implements: `addNode()`, `pushBackNode()`, `findParent()`, `findChild()`, `getChildNode()`, `getNodeIter()`, `next()`. Implements recursive `update()`, `transform()`, `render()` that dispatch to a payload (payload added in Phase 2). `init()` sets `m_headNode`, computes accumulated size/radius by walking children.
- `../library/node/nodedata.h` / `nodedata.cpp` — Port `CNodeData` from Vulkan engine. Holds parsed XML data for one node: name, IDs, type, group, objectName, hasChildrenNodes. Remove Vulkan-specific UI control fields (no `CUIControl` in this engine). Keep `ENodeType` with `_NULL_`, `SPRITE`, `OBJECT` only.
- `../library/node/nodedatalist.h` / `nodedatalist.cpp` — Port `CNodeDataList`. Recursive XML tree → flat vector of `CNodeData` with parent/child IDs. Same `m_idCounter` pattern.
- `../library/node/nodedefs.h` — `ENodeType` enum, `defs_DEFAULT_NODE_ID`, `handle16_t` typedef.

### Validation:
- Compiles as part of the library.
- Unit-level: construct a CNode, add children, verify `findChild()` and `getChildNode()` work.

**⏸ STOP — User review before Phase 2.**

---

## Phase 2 — Payload System

**Goal:** Implement the composition-based payload mechanism so CNode can hold a sprite or transform-only object.

### Design choices:
- `m_payload` is a `std::variant<std::monostate, CSpritePayload, CObjectPayload>`.
- `std::monostate` = grouping node (like a render node with no visual).
- `CSpritePayload` wraps a `CSprite` (constructed from object data, same as current sprite node).
- `CObjectPayload` wraps a `CObject` (transform-only grouping with position/rotation/scale from XML).

### Files to create/modify:
- `../library/node/nodepayload.h` — Define `CSpritePayload` and `CObjectPayload` structs. Each has:
  - `update()` — sprite calls `CSprite::Update()`, object is no-op.
  - `transform()` / `transform(const CObject&)` — delegates to owned object's transform.
  - `render(const CCamera&)` — sprite calls `CSprite::render()`, object is no-op.
  - `getObject()` → returns `CObject*` from the owned sprite or object.
  - `getSprite()` → returns `CSprite*` (sprite payload only, object returns nullptr).
  - `getRadius()` / `getSize()` — delegates to owned data.
- `../library/node/node.h` / `node.cpp` — Add `m_payload` member. Update `update()`, `transform()`, `render()` to use `std::visit` on `m_payload` before recursing children. Add `getObject()`, `getSprite()` accessors that forward to payload.

### Validation:
- Construct a CNode with a sprite payload, call `update()`, `transform()`, `render()` — verify delegation.
- Construct a tree: object node parent, two sprite leaf children. Verify recursive transform propagation.

**⏸ STOP — User review before Phase 3.**

---

## Phase 3 — Node Factory and XML Loading

**Goal:** Implement the factory and data-list-driven construction so node trees can be built from XML data.

### Files to create/modify:
- `../library/node/nodefactory.h` / `nodefactory.cpp` — `NNodeFactory::Create(const CNodeData&)` creates a `CNode*` with the appropriate payload variant based on `CNodeData::getNodeType()`:
  - `SPRITE` → `CSpritePayload` (fetches object data from `CObjectDataMgr`).
  - `OBJECT` → `CObjectPayload` (loads transform from XML node).
  - `_NULL_` → `std::monostate` (pure grouping).
- `NNodeFactory::Create(const CNodeDataList&)` → builds the full tree: creates all nodes, calls `addNode()` to assemble parent-child relationships, calls `init()` on the head node.
- Update `../library/CMakeLists.txt` to include the `node/` subdirectory.

### Validation:
- Create a test XML node definition with nested nodes. Parse with `CNodeDataList`, build with `NNodeFactory::Create()`. Verify tree structure, node types, parent-child relationships.
- Compile and link with the game template.

**⏸ STOP — User review before Phase 4.**

---

## Phase 4 — Game Integration

**Goal:** Wire the node system into the game template so it can be used in game states.

### Work:
- Create a `CNodeMgr` (or integrate into existing manager pattern if appropriate) that follows the `CManagerBase` `LoadListTable`/`LoadGroup` pattern.
- Add node data XML files under `data/objects/` (or a new `data/nodes/` directory — see open questions).
- Update a game state (e.g., `CStartupState` or `CTitleScreenState`) to load and render a node tree as a proof of concept.
- Ensure node trees participate in the existing game loop: `update()` → `transform()` → `render()`.
- Light binding: if the head node contains sprites, `setLights()` should propagate to all sprite payloads in the tree.

### Validation:
- A multi-node tree loads from XML, transforms correctly, and renders on screen.
- Parent transforms propagate to children (move the parent, children follow).

**⏸ STOP — User review before Phase 5.**

---

## Phase 5 — Cleanup and Polish

**Goal:** Remove any scaffolding, finalize the API, and ensure the node system is production-ready.

### Work:
- Verify all memory management: CNode destructor deletes children (or uses `std::unique_ptr<CNode>`).
- Size/radius accumulation in `init()`: walk children, compute bounding size same as Vulkan engine.
- CRC16 name lookup verification across complex trees.
- Review thread safety: handle generation is already atomic; verify no other shared state.
- Document the node system in a brief comment block at the top of `node.h`.
- Remove any temporary test code.

### Validation:
- Full build (debug + release) succeeds.
- No memory leaks (run under Valgrind or ASan if available).
- Node trees with 3+ levels of nesting work correctly.

**⏸ STOP — Final user review.**

---

## Open Questions

1. **UI Controls:** The Vulkan engine has `CUIControlNode` and `CUIControlLeafNode` for meters and progress bars. This software rendering engine has no UI control classes yet. Should the node system include a `CUIControlPayload` variant for future use, or should that be deferred until UI controls are implemented?

2. **Data directory:** Should node XML data live under the existing `data/objects/` tree, or should there be a separate `data/nodes/` directory? The Vulkan engine uses the object data system (`CObjectDataMgr`) for sprites within nodes.

3. **`std::variant` vs. `std::unique_ptr<iNodePayload>`:** The plan uses `std::variant` for zero-overhead dispatch (no heap allocation, no virtual call). The downside is that adding a new payload type requires modifying the variant typedef and recompiling everything that includes `node.h`. An `iNodePayload` interface with virtual methods is more extensible but adds a vtable pointer and heap allocation per node. Which approach is preferred?

4. **Ownership model:** Should `CNode` own its children via `std::vector<std::unique_ptr<CNode>>` (RAII, no manual delete) or raw `std::vector<CNode*>` (matching the Vulkan engine pattern with manual delete in destructor)? `unique_ptr` is safer but prevents sharing nodes across trees.

5. **`CSprite` lifetime in payloads:** `CSpritePayload` will own a `CSprite` by value (or `unique_ptr`). The current `CSprite` constructor takes `const iObjectData&` by reference — should the payload store the object data reference, or should it rely on `CObjectDataMgr` keeping data alive (current pattern)?

6. **Render method name:** The Vulkan engine uses `recordCommandBuffer()` for rendering. This software rendering engine uses `render(const CCamera&)`. The plan uses `render()` — confirm this is the correct method to dispatch.

7. **Physics:** The Vulkan engine nodes have `destroyPhysics()` and `initPhysics()` methods. Does this engine need physics support in nodes, or should that be deferred?

8. **Node manager scope:** Should `CNodeMgr` manage all node trees globally (like `CSoundMgr`), or should individual game states own their node trees directly? The Vulkan engine appears to use both patterns depending on context.
