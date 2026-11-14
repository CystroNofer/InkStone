# Rendering Math Conventions

The maintained explanation is in the
[rendering wiki](docs/RenderingAndUI.md#matrix-and-coordinate-conventions).
This file remains a short reference.

- Vector convention: column vectors
- CPU matrix storage: row-major, indexed as matrix[row][column]
- Transform application: v_clip = P * V * M * v_local
- World handedness: left-handed
- Camera/view space handedness: left-handed
- Camera forward in view space: +Z
- NDC X Y range: [-1, 1]
- NDC Z range: [0, 1]
- UV origin: bottom-left
- UV +U direction: right
- UV +V direction: up
- Front-face winding: not explicitly configured by the current renderer; world
  handedness and cross-product conventions do not define rasterizer winding.
- OpenGL depth: the renderer converts engine NDC Z from [0, 1] to [-1, 1].
- Local transforms: TRS; child world matrix = parent world matrix * child local matrix.
