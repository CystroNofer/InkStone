# Architecture

[Wiki](README.md) · [Scene](Scene.md) · [Assets](Assets.md) · [Rendering and UI](RenderingAndUI.md)

## Implemented module boundaries

The build currently produces one `engine` console application, not separate
engine/editor executables. `APISetting` selects OpenGL and GLFW; alternative
backend names are not evidence of implemented backends.

| Module | Responsibility | Entry points |
| --- | --- | --- |
| Application | Startup, frame orchestration, demo content, viewport and dock layout | [Application.cpp](../engine/src/Application.cpp) |
| Window / Input / Event | GLFW context, callbacks, polling, input state, queued events | [Window](../engine/src/Window/Window.h), [Input](../engine/src/Input/Input.h), [EventBuffer](../engine/src/Event/EventBuffer.h) |
| ECS / Gameplay | Entity lifetime, component storage, hierarchy, transform propagation, render-item collection | [Scene](../engine/src/ECS/Scene.h), [components](../engine/src/Gameplay/Component) |
| Asset | Shader/texture/material ownership and static-model loading | [Asset directory](../engine/src/Asset) |
| Rendering | Backend interfaces, GPU objects, materials, and draw submission | [GraphicsDevice](../engine/src/Rendering/GraphicsDevice.h), [Renderer](../engine/src/Rendering/Renderer.h) |
| UI | Frame lifecycle and reusable widgets; OpenGL/ImGui implementation behind the interface | [UI](../engine/src/UI/UI.h), [OpenGLUI](../engine/src/UI/OpenGL/OpenGLUI.cpp) |
| Math / Container | Engine vectors, matrices, quaternions, generational resource handles, sparse storage | [Math](../engine/src/Math), [Container](../engine/src/Container) |

ECS storage and scene render-item collection are integrated into `Scene`. A
separate registry or scene renderer is not part of the active application path.
Likewise, the presence of `Layer`/`LayerStack` source does not mean the current
application drives its frame through a layer stack.

ModelLoader handles file import and GPU upload. Scene includes the model data
definitions from `Model.h` and owns `InstantiateModel`, but does not include or
call ModelLoader. Application coordinates these stages. Shared validation lives
in an internal helper used by both implementations.

## Startup and frame flow

Startup creates the window and current OpenGL context first, then initializes
the renderer/device, input, and UI. The application creates a camera, framebuffer,
demo mesh, shader, material, texture, and scene.

The ordinary frame follows this order:

1. `Input::Update()` polls GLFW events and refreshes input state.
2. Application handles queued events and forwards events to UI, then clears the queue.
3. Time and the application-controlled orbit camera are updated.
4. `UI::NewFrame()` begins UI recording. Application builds the dock layout,
   viewport image, scene-information window, and hierarchy widget.
5. If the viewport is usable, Application binds its framebuffer and calls
   `Scene::Render(camera, cameraTransform)`.
6. Scene updates world matrices, collects render items, and submits the list to Renderer.
7. The framebuffer is unbound. `UI::EndFrame()` submits ImGui draw data, then the
   window swaps buffers.

Recording the viewport image before rendering the scene is intentional: the
image is sampled later, when ImGui's recorded draw commands are submitted.

## Ownership and borrowing

| Object | Current owner | What consumers retain |
| --- | --- | --- |
| Scene | Application's `unique_ptr<Scene>` | References or a borrowed hierarchy view |
| Components | Scene's type-specific sparse stores | Short-lived pointers/references |
| Imported CPU geometry | `ModelData` value | Values/references chosen by the caller |
| Uploaded mesh | Shared by `Model`, `Renderable`, and temporary `RenderItem` objects | `shared_ptr<Mesh>` |
| Mesh VA and index buffer | Mesh's `unique_ptr` members | Borrowed access |
| OpenGL vertex buffer | Shared ownership held by the vertex array | Existing shared owner, not a new owner made from `.get()` |
| Shader / Material / Texture2D | Static manager `HandleMap`s holding `unique_ptr`s | Non-owning `Handle<T>` values; nullable borrowed pointers from `Get()` |
| Window | WindowManager's handle map | Window handle or borrowed pointer |
| Device / Renderer / UI | Their static owning instances | Static interface or borrowed device pointer |

An entity handle and an asset handle are different types with different rules.
Neither keeps its target alive. A mesh `shared_ptr` does keep its target alive.

Factories still commonly return owning raw pointers. The receiver must adopt
each result exactly once. For example, the raw `Renderable(Mesh*, ...)`
constructor takes ownership; it must not receive `.get()` from an already-owned
mesh. Use the shared-pointer overload for that case. These mixed ownership
interfaces are transitional, not a blanket convention for new APIs.

## Known lifetime and threading limitations

GPU creation, use, and final destruction require an appropriate current OpenGL
context. Removing an entity does not necessarily destroy its mesh: another
instance or a retained `Model` may still own it.

The required shutdown ordering is to release scene/model GPU owners,
framebuffers, manager resources, and UI backend resources while the context is
alive, then destroy the window/context. **This is not yet a coordinated shutdown
procedure in the application.** Static managers and an empty Application
destructor do not establish that ordering across translation units. `OpenGLUI`
also currently has an empty destructor.

Scene mutation, resource managers, rendering, and UI are not documented as
thread-safe. `IterationGuard` protects synchronous scene iteration from
structural mutation; it is not a mutex. CPU-only import is separated from GPU
upload, but no asynchronous loading service has been implemented.

Additional ownership gaps and planned work are recorded in
[Decisions and backlog](Decisions.md).

