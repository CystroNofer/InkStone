# Scene and hierarchy

[Wiki](README.md) · [Architecture](Architecture.md) · [Assets](Assets.md)

Source: [Scene.h](../engine/src/ECS/Scene.h),
[Scene.cpp](../engine/src/ECS/Scene.cpp),
[Entity.h](../engine/src/ECS/Entity.h),
[SparseSet.h](../engine/src/Container/SparseSet.h),
[Transform.cpp](../engine/src/Gameplay/Component/Transform.cpp).

## Entities and components

**Implemented:** `Scene` owns entity lifetime, per-type component stores,
hierarchy links, cached world matrices, and render-item collection. Scene is
noncopyable.

`Entity` is an assignable ID/generation value. Its identity fields and constructor
are private; Scene creates valid handles. Copying a handle does not copy an
entity. Reassigning a handle variable does not alter the entity it used to name.

- `NewEntity(name)` creates a parentless entity with `NameTag` only.
- `NewEntity<Transform>(name)` explicitly adds a default identity transform.
- `AddComponent<C>(entity, args...)` forwards constructor arguments. An lvalue
  handle can be copied into a component without being moved from.
- `TryGetComponent<C>(entity)` returns a borrowed pointer, or null for a stale
  handle or absent component. The const overload gives read-only component access.
- `DestroyEntity(entity)` removes the entire descendant subtree, child-first.

Do not retain component pointers across structural changes: packed storage can
move components. The API currently has no public individual-component removal
operation.

Liveness checks require an in-range ID, matching generation, and an occupied
slot. Destruction increments generation before ID reuse; an exhausted generation
retires the slot instead of wrapping. **Handles are scene-local:** no scene
identity is embedded, so a handle from another scene can accidentally match.

## Spatial hierarchy

Each live entity has an occupied entry in
`vector<optional<HierarchyNode>> m_Hierarchy`. A record stores parent, children,
and a cached world matrix. Empty entries represent destroyed entities. This
storage is indexed directly by entity ID; it is not an unordered map.

Record occupancy and spatial participation are different:

- An entity without `Transform` can hold nonspatial data.
- `SetParent(child, parent)` requires transforms on the child and any supplied parent.
- `GetRoots()` includes only live, parentless entities with transforms.
- Rendering requires both `Renderable` and `Transform`.

There is no separate `m_Roots` cache. `GetRoots()` scans hierarchy slots and
returns an owning vector snapshot in ascending entity-ID order. This avoids
keeping a redundant root list synchronized, at the cost of a scan and allocation.
Child vectors still store explicit sibling order; attaching a child appends it.

### Parenting behavior

`SetParent(child, parent)` rejects stale handles, missing transforms,
self-parenting, cycles, and changes attempted during `Each()`. It preserves the
child's **local** position, rotation, and scale. Its world-space placement may
therefore change. Passing `std::nullopt` detaches the child and makes it a root.

Keep-world reparenting is not implemented. For example, if a parent is translated
by 10 units and its child has local X = 2, the child's world X becomes 12;
detaching it with keep-local semantics leaves local X = 2 and makes world X = 2.

`GetParent()` returns `nullopt` both for a root and for an invalid entity. Use
`IsAlive()` when the distinction matters. `GetChildren()` returns an empty span
for invalid entities.

## Transforms and cached world matrices

`Transform` stores public local position, quaternion rotation, and scale.
`GetMMatrix()` builds `T * R * S`; it does not walk the hierarchy.

`UpdateWorldTransforms()` traverses spatial roots and descendants with an
explicit stack, evaluating:

```text
root.world  = root.local
child.world = parent.world * child.local
```

`GetWorldMatrix()` returns a copied snapshot, or `nullopt` for a stale/nonspatial
entity. It does not refresh the cache. Call `UpdateWorldTransforms()` after
local edits or reparenting when reading world matrices outside rendering.
New records start with an identity cache, which may not reflect their current TRS.

`Scene::Render()` refreshes world matrices every time. There is no dirty tracking:
public TRS fields can currently be edited without notifying Scene. A dirty flag
would need a reliable mutation-notification policy before it could be correct.

Local transforms are TRS-only. Multiplication through a hierarchy can still
produce world-space shear, which the cached matrix retains. This does not make
an arbitrary imported **local** sheared matrix exactly representable; see
[import conversion](Assets.md#trs-conversion-policy).

## Component iteration

`Each<Cs...>(callback)` iterates the smallest requested component store and checks
membership in the others. The callback receives `(EntityID, Cs&...)`, not a full
generation-bearing `Entity`. Use at least one component type. If a requested
store does not exist, the current implementation warns and returns.

`IterationGuard` increments `m_IterationDepth` on entry and decrements it on
scope exit, including exception unwinding. Nested iteration is allowed.

| Operation inside `Each()` | Behavior |
| --- | --- |
| Read or edit existing component fields | Allowed |
| Nested `Each()` | Allowed |
| `NewEntity()` | Throws `std::logic_error` |
| `AddComponent()`, `SetParent()`, `DestroyEntity()` | Returns false |

Queue structural operations in caller-owned data and apply them afterward.
The guard does not protect arbitrary manual traversal, nor does it provide
thread synchronization.

## Borrowed hierarchy view

`SceneHierarchyView`, declared beside Scene, exposes read-only hierarchy access
without exposing component mutation to UI:

| Method | Result and lifetime |
| --- | --- |
| `GetRoots()` | Owning vector snapshot; its handles can later become stale |
| `GetChildren(entity)` | Borrowed `span<const Entity>` into a child vector |
| `GetName(entity)` | Borrowed `string_view`, or the fallback `Unnamed Object` |
| `GetIdentity()` | Opaque scene address for scoping UI state, not a persistent ID |

The scene must outlive the view. Do not change structure or names while consuming
the returned spans/string views. Read-only access does not prevent a different
caller from invalidating them. `GetHierarchyView()` on a temporary Scene is
deleted to prevent one obvious dangling-view case.

## Model instantiation

`InstantiateModel(model, material, parent)` returns
`std::expected<Entity, ModelError>` containing the instance root on success.
It consumes already-uploaded Model data, creates the hierarchy, and shares
meshes with its Renderables; it does not import files or allocate GPU geometry.
The optional parent must be a live spatial entity in this Scene. Do not call
during `Each()`. See [Assets](Assets.md#scene-instantiation) for the complete
creation and cleanup policy.

Scene includes `Asset/Model.h`, not `Asset/ModelLoader.h`. File loading and GPU
upload remain caller-controlled operations outside Scene.

## Rendering and camera limitation

Scene collects `Renderable`/`Transform` pairs into `RenderItem`s containing a
shared mesh, material handle, and world matrix. Renderer receives the whole list
and controls material binding.

The current render call takes a camera and camera transform from Application.
Its view matrix is a look-at-origin placeholder based on camera position;
camera rotation is not used. `m_MainCamera` exists but is not a working camera
selection API. Scene-camera selection and hierarchy-aware camera transforms
remain deferred.

