# Assets and model loading

[Wiki](README.md) · [Scene](Scene.md) · [Rendering and UI](RenderingAndUI.md) · [Building](Building.md)

Source: [Model.h](../engine/src/Asset/Model.h),
[ModelLoader.h](../engine/src/Asset/ModelLoader.h),
[ModelLoader.cpp](../engine/src/Asset/ModelLoader.cpp),
[ModelValidation.h](../engine/src/Asset/ModelValidation.h),
[Scene.cpp](../engine/src/ECS/Scene.cpp),
[Renderable.h](../engine/src/Gameplay/Component/Renderable.h),
[Assimp integration](../external/Assimp/README.md).

## Three separate operations

**Implemented:** `ModelLoader` is a stateless import/upload utility. It has no
mesh handle map, persistent cache, or entity-instantiation API. Scene consumes
the resulting Model through `Scene::InstantiateModel`; it includes `Model.h`,
not `ModelLoader.h`. Both implementations reuse the internal model-validation
helpers, so Scene does not depend on loader implementation details.

| Operation | Input to output | Requirements and effects |
| --- | --- | --- |
| `ModelLoader::Import` | File path to `ModelData` | CPU-only Assimp parsing and conversion; no scene or GPU objects |
| `ModelLoader::Upload` | `ModelData` to `Model` | Initialized device and current graphics context on the graphics thread; creates GPU meshes |
| `Scene::InstantiateModel` | `Model`, material, optional parent to root Entity in that Scene | Creates scene entities and shares uploaded meshes; no new GPU geometry |

All three return `std::expected<result, ModelError>`. Check the result before
dereferencing it. `ModelError` contains source, stage (`import`, `upload`, or
`instantiate`), and message. Preconditions and ordinary caught exceptions are
reported this way; it is not a universal no-throw guarantee under memory exhaustion.

CPU data can be inspected, edited, or discarded independently of GPU upload.
Uploading the same `ModelData` again creates another set of GPU meshes. To reuse
GPU geometry, instantiate the same uploaded `Model` multiple times.

## Data vocabulary

| Type | Purpose |
| --- | --- |
| `ModelVertex` | CPU position, UV0, and normal |
| `MeshData` | Named vertex/index arrays for one geometry |
| `ModelTRS` | Cached local translation, quaternion rotation, and signed scale |
| `ModelNode` | Named placement/grouping node: parent index, local TRS, and mesh indices |
| `ModelData` | Source path, CPU meshes, nodes, and import warnings |
| `Model` | Source path, shared GPU meshes, copied nodes, and warnings |
| `ModelWarning` | Nonfatal context/message retained for inspection |
| `ModelImportSetting` | Optional UV flip and TRS conversion tolerance |

A mesh is geometry; a node is placement and hierarchy. Several nodes can
reference one mesh. One node can reference several meshes. Indices in model
data are asset-local indices, never Scene entity IDs.

Node zero is the sole imported root, with no parent. Every other node's parent
must precede it. Upload and instantiation validate this layout and mesh indices.
These structs are public mutable values, not an enforced immutable asset format.

## Import policy

The importer uses the Assimp C API and copies retained data into engine-owned
storage. The Assimp scene is released through `aiReleaseImport` before `Import`
returns. No pointers into it escape.

- Triangulate and validate Assimp data first.
- When normals are missing, request smooth-normal generation before vertex
  joining; normalize retained normals. Generated smoothing may differ from the
  author's intended hard edges.
- Retain positions, normals, and UV channel zero. Missing UV0 becomes `(0, 0)`
  with a warning. Non-2D UV0 is converted to two components.
- Warn when discarding vertex colors, tangents, or additional UV channels.
- Ignore file materials entirely. The application supplies one material per
  instantiation, used by every created Renderable.
- Ignore animation channels, imported cameras, and lights with warnings.
  Mesh skinning and morph targets are rejected, not silently treated as supported.
- Reject unusable geometry, nonfinite values, invalid indices, and remaining
  point/line primitives. Triangulation does not turn every primitive into triangles.
- Preserve Assimp's output coordinates and units without additional engine
  handedness/unit conversion. Recognized axis/unit metadata produces a warning;
  absence of that warning is not proof that a model matches engine conventions.

`flipUVs` defaults to false. This changes mesh UVs when enabled; it is separate
from the image loader's vertical pixel flip.

Assimp supplies format parsing, including the intended OBJ/FBX/STL use cases.
An extension supported by Assimp does not mean InkStone supports every feature
inside that file. This pipeline is specifically for static triangle geometry.

## TRS conversion policy

Import calls Assimp's `aiDecomposeMatrix` for each node's **local** matrix and
normalizes the returned quaternion. It stores the result in `ModelTRS`, so the
same unchanged model reuses that result at every instantiation.

The importer then recomposes TRS with the engine's `Transform::GetMMatrix()` and
compares the original and reconstructed 3x3 linear blocks. The error metric is:

```text
max absolute entry difference / max(max absolute original entry, 1e-30)
```

`trsTolerance` defaults to `1e-4`; it must be finite and strictly between zero and
one. Comparing the linear block separately prevents large translations from
hiding discarded shear.

| Condition | Current policy |
| --- | --- |
| Error exceeds tolerance | Warn and retain the approximate TRS; descendant placement can change |
| Quaternion needed significant normalization | Warn and retain the normalized result |
| Zero or near-zero scale | Warn about ambiguous rotation and unstable inverse normal transforms |
| Product of decomposed scales is negative | Warn about reflection; retain signed scale |
| Nonfinite/non-affine matrix, unusable quaternion, or overflowing TRS | Fail import |

There is no original-matrix fallback, geometry baking, or automatic winding
correction. A warning is not an exact-conversion guarantee. Later edits to
public model fields also do not regenerate the original import warnings.

## GPU layout and lifetime

Upload explicitly packs each vertex as eight floats:

| Location | Attribute | Components | Byte offset on the current build |
| --- | --- | --- | --- |
| 0 | `a_PositionOS` | 3 floats | 0 |
| 1 | `a_TexCoord` | 2 floats | 12 |
| 2 | `a_NormalOS` | 3 floats | 20 |

Stride is 32 bytes; indices are unsigned 32-bit values on the current Windows
build. Packing does not assume the CPU vector structs have that binary layout.
Each imported mesh gets one uploaded Mesh. Current count-based APIs impose
conservative per-buffer limits below 4 GiB, with an additional signed draw-count
limit for indices.

`Model` retains shared Mesh owners, not CPU vertex/index arrays. Upload does not
erase the caller's `ModelData`. Each Mesh owns its VA and index buffer; the VA
retains the vertex buffer. Partial upload owners are released on failure.

All final GPU releases must happen while the appropriate context is alive.
Destroying a Model alone does not release meshes still used by live instances;
destroying an instance alone does not release meshes still owned by a Model or
another instance.

## Scene instantiation

`Scene::InstantiateModel` creates an extra identity-transform instance root, optionally
parented under an existing spatial entity. It then creates one entity per
imported node, copies cached local TRS, and preserves parent relationships.

- A node with no meshes remains a grouping entity.
- A node with one mesh gets one Renderable directly.
- A node with multiple meshes gets one identity-transform child per mesh,
  each with one Renderable. The imported node remains their parent.

Consequently each Renderable holds exactly one shared mesh. Geometry is not
duplicated merely to represent another placement. Deleting the returned
instance-root entity removes the complete instance subtree.

Supply a valid material and, if present, a live parent with Transform from the
same scene. Do not call during `Scene::Each()`. The implementation tracks new
entities and attempts child-first cleanup if construction fails. World-matrix
caches are refreshed by the next render, or an explicit `UpdateWorldTransforms()`.

The following example assumes an initialized graphics context and an existing
material. It uses the API from application code, not a command-line import mode:

```cpp
#include "Asset/ModelLoader.h"
#include "ECS/Scene.h"

std::expected<NXTN::Entity, NXTN::ModelError> AddModel(
    NXTN::Scene& scene,
    const std::string& path,
    NXTN::Handle<NXTN::Material> material)
{
    std::expected<NXTN::ModelData, NXTN::ModelError> data =
        NXTN::ModelLoader::Import(path);
    if (!data) { return std::unexpected(data.error()); }

    std::expected<NXTN::Model, NXTN::ModelError> model =
        NXTN::ModelLoader::Upload(*data);
    if (!model) { return std::unexpected(model.error()); }

    return scene.InstantiateModel(*model, material);
}
```

The instance's Renderables keep the meshes alive after this function returns.
Keep `Model` outside the function instead when repeated instantiation or later
warning inspection is needed. Warnings are logged during import and retained
in both ModelData and Model; no warning-inspector UI exists yet.

## Other asset managers

ShaderManager, TextureManager, and MaterialManager own objects through static
`HandleMap`s. Their `Get()` results are borrowed, nullable pointers; do not delete
them. Material stores a shader handle and texture handles, not ownership of those
resources.

`HandleMap::Validate()` currently checks ID range and generation, not slot
occupancy. For usable-resource checks, inspect `Get()` as well. Resource-handle
fields are public, unlike Entity's private identity, and their generation-wrap
policy is not the same as Scene's.

**Not implemented:** model caching, asynchronous scheduling, hot reload,
serialization, automatic file-material conversion, animation, or an import UI.

