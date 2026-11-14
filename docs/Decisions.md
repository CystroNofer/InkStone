# Decisions and backlog

[Wiki](README.md) · [Architecture](Architecture.md)

This page records current choices and deferred work, not promises about future
features. Implementation details and source links live on the linked topic pages.

## Implemented decisions

| Choice | Rationale and trade-off |
| --- | --- |
| Integrate registry responsibilities and scene render collection into Scene | Keeps the current scene workflow direct. Scene now depends on both ECS and rendering concepts. |
| Make Entity assignable, with private identity and construction | Ordinary containers can move/erase handles without letting callers edit their fields or construct arbitrary identities. Handles remain scene-local. |
| Make Transform optional | Supports nonspatial data entities. Parenting and rendering explicitly require spatial components. |
| Use keep-local reparenting | Preserves editable local TRS without decomposing a new matrix; world placement can change. |
| Derive roots rather than cache them | Removes one redundant list/invariant; every root query scans hierarchy slots and allocates a snapshot. |
| Reject structural changes during Each | Protects packed-storage iteration; callers must defer structural work themselves. |
| Give UI a borrowed hierarchy view | Exposes only read access needed by the widget; requires explicit lifetime and invalidation rules. |
| Separate model import, upload, and instantiation | Allows CPU inspection and reuse of one GPU model across multiple scene instances. |
| Keep import/upload in ModelLoader and instantiation in Scene | Scene depends on Model data, not the loader API; ModelLoader's public interface no longer depends on Entity or Scene. Shared validation avoids duplicating the model invariants. |
| Cache Assimp-decomposed local TRS | Avoids repeated decomposition and fits engine Transform; local shear can be lost, with warnings. |
| Share geometry; keep one mesh per Renderable | Multiple placements reuse meshes. Multi-mesh imported nodes require extra renderable child entities. |
| Ignore imported materials for now | Keeps geometry loading independent of file-material conversion; one application material is used per instantiation. |
| Use separate shader-stage files | Ordinary GLSL sources and explicit stage descriptors replace custom `#type` parsing. |
| Let Renderer bind materials from complete RenderItems | The caller need not establish an implicit material binding before drawing. |

## Known gaps to address

These are code-level observations, not fixes performed by creating this wiki.

| Area | Current gap | Relevant page/source |
| --- | --- | --- |
| Shutdown / ownership | Static resources lack coordinated release before context destruction; raw adopting factory interfaces coexist with smart ownership. OpenGLUI's destructor does not shut down its backend/context. | [Architecture](Architecture.md#known-lifetime-and-threading-limitations) |
| Resource handles | HandleMap validation does not check occupied slots, and generation wrap is not handled like Entity generation exhaustion. | [Assets](Assets.md#other-asset-managers) |
| Material failure handling | Material construction dereferences an unresolved shader; invalid texture binding leaves no defined fallback. | [Rendering](RenderingAndUI.md#shaders-and-materials) |
| Shader metadata | Fixed name buffer, omitted initial UInt reflection, and incomplete type/array/block coverage. | [Rendering](RenderingAndUI.md#shaders-and-materials) |
| Vertex layouts / draw ordering | No general integer-input, padding, or explicit-location descriptors; no transparency sort or bind-suppression batching. | [Rendering](RenderingAndUI.md#vertex-layouts) |
| Cameras | Application supplies an external camera; view calculation ignores its rotation. Main-camera selection is not implemented. | [Scene](Scene.md#rendering-and-camera-limitation) |
| Imported coordinates | No additional engine axis/unit conversion or reflection-culling policy; lossy local TRS is accepted with warnings. | [Assets](Assets.md#trs-conversion-policy) |
| UI | Hierarchy display only; input forwarding and backend-independent application UI are incomplete. | [UI](RenderingAndUI.md#ui-boundary-and-hierarchy-drawing) |
| Event ownership | Events are allocated as raw pointers; `EventBuffer::Clear()` clears pointers without deleting events, and overflow does not release rejected events. | [EventBuffer.cpp](../engine/src/Event/EventBuffer.cpp), [Application.cpp](../engine/src/Application.cpp) |
| Quaternion helpers | `normalize(Quaternion)` passes components to the constructor in the wrong order. `Rotate()` overwrites operands in place and assigns `y` twice instead of writing `z`. Imported TRS explicitly normalizes/copies components and does not use these helpers. | [Quaternion.cpp](../engine/src/Math/Quaternion.cpp) |

## Proposed work, not implemented

- Measure hierarchy occupancy and traversal costs before replacing direct-ID
  storage with a map or another representation.
- Choose a transform-change notification mechanism before introducing dirty
  world-matrix tracking. Public field mutation currently bypasses notification.
- Define transform-removal rules before adding individual component removal;
  consider debug assertions for parent/child and spatial-component invariants.
- Add main-camera selection and use hierarchy-derived camera transforms.
- Finish resource ownership, error handling, and explicit shutdown ordering.
- Consider scene identity in Entity handles if cross-scene misuse becomes a concern.
- Extend shader/material reflection and parameter inspection before adding more
  uniform types or hot reload.
- Consider model caching, asynchronous loading, and an import-warning inspector
  after the synchronous pipeline and lifecycle contracts are stable.

Keep proposals here until implemented. Promote them to the relevant design page
when the code actually supports them, and remove the corresponding stale limitation.

