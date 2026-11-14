# InkStone

InkStone is an experimental C++23 GPU renderer and scene/editor prototype. The
current application targets Windows x64 with OpenGL 4.5, GLFW, Dear ImGui, and
Assimp. Engine types live in the `NXTN` namespace.

Start with the [design wiki](docs/README.md) or the
[build and run instructions](docs/Building.md).

## Current scope

- A scene-owned ECS, optional transforms, and a spatial entity hierarchy.
- Indexed mesh rendering with shaders, materials, and 2D textures.
- Static model import, separate GPU upload, and hierarchy-preserving instantiation.
- An offscreen scene viewport and a read-only hierarchy widget.

The startup application currently displays a hard-coded textured quad. Model
loading is available through `ModelLoader`, with instantiation through
`Scene::InstantiateModel`, but is not wired into startup or a file-picker UI.
Imported file materials, animation, and additional graphics
backends are not implemented.

This is an actively changing prototype, not a stable engine API. The wiki records
the current implementation, its limitations, and deferred decisions separately.

## Documentation

| Topic | Page |
| --- | --- |
| Modules, frame flow, and ownership | [Architecture](docs/Architecture.md) |
| Entities, components, parenting, and borrowed views | [Scene and hierarchy](docs/Scene.md) |
| Assimp, model data, GPU upload, and instantiation | [Assets and model loading](docs/Assets.md) |
| Draw submission, uniforms, textures, and UI | [Rendering and UI](docs/RenderingAndUI.md) |
| Premake, Visual Studio, and dependencies | [Building](docs/Building.md) |
| Design rationale and outstanding work | [Decisions and backlog](docs/Decisions.md) |

