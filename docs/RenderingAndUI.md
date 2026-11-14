# Rendering and UI

[Wiki](README.md) · [Architecture](Architecture.md) · [Scene](Scene.md) · [Assets](Assets.md)

## Draw submission

Source: [Renderer.h](../engine/src/Rendering/Renderer.h),
[OpenGLRenderer.cpp](../engine/src/Rendering/OpenGL/OpenGLRenderer.cpp),
[GraphicsDevice.h](../engine/src/Rendering/GraphicsDevice.h).

Scene collects render items, but does not bind their materials. Each item carries
a shared mesh, material handle, and world matrix. `Renderer::DrawMesh` receives
the whole vector; the OpenGL implementation sorts it **in place** by material
handle `(id, generation)`.

For each item it resolves the material, skips missing meshes/materials, sets
`u_ModelMatrix` and `u_VPMatrix`, binds the material and mesh, and issues indexed
triangle drawing. The material is bound and its uniforms uploaded for every
draw; grouping does not yet eliminate repeated state changes or merge draws.

Depth testing uses `GL_LESS`; alpha blending is enabled. There is no transparent
depth-sort pass and no explicit face-culling policy in this renderer. Material
ordering is not correct general-purpose transparency ordering. These are known
limitations, not guarantees supplied by the material sort.

## Matrix and coordinate conventions

Source: [Matrix.h](../engine/src/Math/Matrix.h),
[Transform.cpp](../engine/src/Gameplay/Component/Transform.cpp),
[Camera.cpp](../engine/src/Gameplay/Component/Camera.cpp),
[Texture.vert](../asset/Shader/Texture.vert).

| Topic | Current convention or behavior |
| --- | --- |
| Vector application | Column vectors: `clip = P * V * M * local` |
| CPU matrix storage | Row-major `m[row][column]`; independent of column-vector math |
| Local transform | `T * R * S`, rotation stored as quaternion `(x, y, z, w)` |
| Hierarchy | `world = parentWorld * local` |
| World/view convention | Left-handed convention, camera forward in view space is +Z |
| Engine NDC | X/Y in `[-1, 1]`, Z in `[0, 1]` |
| OpenGL adjustment | Pre-multiply VP by a correction mapping depth to `[-1, 1]` |
| Matrix uniform upload | `glUniformMatrix4fv` uses `GL_TRUE` transpose for CPU storage |
| UV convention | Bottom-left origin, U right, V up |

Perspective Camera construction accepts field of view in degrees, converts it
to radians, and stores radians in `size`. Orthographic `size` is a spatial
half-height, not an angle. Quaternion axis-angle construction and `Rotate`
currently take **radians** directly. Do not infer one universal angle-unit rule
for every math API from Camera's constructor.

World handedness, cross-product formulas, and rasterizer front-face winding are
separate choices. The old description "left-handed cross product as normal"
does not specify a rasterizer winding convention. The current renderer does
not set `glFrontFace` or enable face culling. Reflected transforms need an
explicit parity policy when culling is implemented.

## Vertex layouts

Source: [VertexArray.h](../engine/src/Rendering/VertexArray.h),
[OpenGLVertexArray.cpp](../engine/src/Rendering/OpenGL/OpenGLVertexArray.cpp).

`VertexArrayLayout` describes tightly packed interleaved attributes. Locations
are assigned sequentially by attribute order; names are not looked up in the
shader. Offsets accumulate `count * type-size`, and stride is their sum.

The OpenGL implementation always uses `glVertexAttribPointer` with normalization
disabled. This suits the current float layouts. It is not yet a general layout
system for padding, explicit locations, matrices, instancing, or integer shader
inputs. Integer inputs need the appropriate integer attribute path, not just an
`Int` enum entry. Imported models use the [fixed model layout](Assets.md#gpu-layout-and-lifetime).

## Shaders and materials

Source: [Shader.h](../engine/src/Rendering/Shader.h),
[OpenGLShader.cpp](../engine/src/Rendering/OpenGL/OpenGLShader.cpp),
[Material.h](../engine/src/Rendering/Material.h),
[Material.cpp](../engine/src/Rendering/Material.cpp).

`ShaderProgramDescriptor` specifies explicit stage/path pairs. Current stages
are vertex and fragment, with ordinary separate GLSL files. There is no custom
`#type` multi-stage parser. Duplicate stages, file errors, compile failures, and
link failures are logged; the factory rejects an invalid program.

OpenGLShader stores uniform metadata by owning string name: type and GL location.
`GetUniformList()` constructs an owning vector of name/type descriptors. This
does not expose GL locations or borrowed unordered-map iterators. Its order is
unspecified, and it reflects active uniforms, not every declaration in source.

Material takes a shader handle and builds its own name-to-parameter map from
that metadata. Values use a variant: int, unsigned int, float, vec3, vec4, mat4,
or texture handle. Named setters check existence/type before updating cached
values. `Bind()` binds the shader and uploads those values.

Known limitations:

- Material construction currently dereferences the resolved shader without a
  null check. Callers must check `ShaderManager::Get()` after loading and before
  creating a Material; the demo startup does not yet handle this robustly.
- Shader setters do not themselves type-check; they rely on their caller and
  require the correct program to be bound.
- Reflection uses a fixed 64-byte name buffer and does not preserve uniform
  array sizes. It is not a complete uniform-array/block interface.
- Initial reflection omits unsigned-int uniforms even though later lookup and
  setters recognize them. Bool, vec2, and cube samplers are reflected but not
  supported as Material values.
- Material metadata is collected at construction, with no hot-reload refresh.
- Per-draw matrices currently live in the same material parameter map as
  reusable material values; the renderer overwrites them for each object.

## Textures and sampler units

Source: [OpenGLTexture.cpp](../engine/src/Rendering/OpenGL/OpenGLTexture.cpp),
[TextureManager.cpp](../engine/src/Asset/TextureManager.cpp).

TextureManager asks GraphicsDevice to create a texture and takes ownership only
on success. Image-load failure logs the path and stb_image reason; the device
rejects the invalid object and the manager returns an invalid handle. There is
no automatic fallback/checkerboard texture.

Current image loading flips pixels vertically, supports 1/3/4 channels, sets
nearest filtering and clamp-to-edge wrapping, and uploads one mip level. Upload
temporarily uses unpack alignment 1 and restores the previous alignment. These
settings are fixed policy, not caller-supplied texture descriptors.

During Material binding, valid textures receive consecutive texture units from
zero. Texture binding uses that unit; the shader sampler receives the same unit
via `glUniform1i`. A unit is **not** an OpenGL texture object ID or a permanent
property of the texture. Unordered material iteration means unit assignments
should not be treated as stable external identifiers.

There is currently no texture-unit-limit check. An invalid texture logs and
skips that binding; it does not install a safe fallback or clear stale sampler
state. Threaded image loading also needs an explicit policy for stb_image's
global flip setting.

## UI boundary and hierarchy drawing

Source: [UI.h](../engine/src/UI/UI.h),
[UI.cpp](../engine/src/UI/UI.cpp),
[OpenGLUI.cpp](../engine/src/UI/OpenGL/OpenGLUI.cpp),
[Application.cpp](../engine/src/Application.cpp).

The base UI interface owns frame entry points and dispatches widget calls to the
backend. Actual hierarchy drawing is in OpenGLUI; base UI does not implement
ImGui widgets. Application still directly uses ImGui for windows, docking, and
the viewport image. Thus the widget boundary exists, but Application is not
fully UI-library-independent.

Within an active frame and caller-owned UI container, call:

```cpp
NXTN::UI::HierarchyTree(scene.GetHierarchyView());
```

The widget displays the spatial hierarchy only: nonspatial entities are omitted.
It supports expansion, not selection, editing, drag/drop parenting, or deletion.
It obtains a root snapshot and borrows child spans and names while drawing.

An explicit `TraversalFrame` stack stores a sibling span and next-child index.
It resumes the correct sibling list after descending into expanded children,
without recursive C++ calls. Indentation is paired with stack push/pop. Widget
identity is scoped by scene address, entity ID, and generation, independently of
labels and ancestor paths. It is not persistent scene serialization identity.

Input forwarding is partial: current OpenGLUI handles mouse/resize events but
leaves key press/release cases empty. Backend/context shutdown and complete
input integration remain outstanding.

