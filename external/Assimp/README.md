# Assimp 6.0.5 (Windows x64)

The existing `assimp-vc143-mt.dll` and optional `.pdb` are from:
https://github.com/assimp/assimp/releases/download/v6.0.5/windows-x64-v6.0.5.zip

The DLL was verified byte-for-byte against that archive (SHA-256):
`253E2B63978A3AD42CC32CE7659DC078E9CCC64392DFA42E7AE3D08322922EC1`.

`include/assimp/` and `LICENSE` are unmodified files extracted from:
https://github.com/assimp/assimp/archive/refs/tags/v6.0.5.zip

See [FILES.md](FILES.md) for the complete integration file inventory.

## Build integration

The release ZIP contains neither headers nor an import library. Before compiling,
Premake's generated project runs `Prepare.ps1` with the active MSVC tools. It:

1. Verifies the DLL hash to prevent mixing unrelated headers and binaries.
2. Generates single-precision `assimp/config.h` from the upstream template.
3. Extracts the DLL's exported `ai*` function names and uses Microsoft's `lib.exe`
   to generate an x64 **C API only** import library, `assimp-c-api.lib`.

Generated files live under each configuration's `bin-obj` directory. There is no
Assimp compilation, CMake requirement, network download, or global installation
during a build. The post-build step copies the DLL beside `engine.exe`.
The PDB is optional and is not needed to compile, link, or run the engine.

Use `<assimp/cimport.h>` and other C API headers from engine implementation files.
`ASSIMP_DLL` is configured by Premake. The release DLL is used for both engine
configurations; the engine's existing `/MT` and `/MTd` settings are unchanged.
Only plain data crosses the DLL boundary. Release imported scenes using
`aiReleaseImport`, never `delete` or `free`, and copy retained data into engine-owned
storage before releasing a scene. Do not call Assimp's C++ API (`Assimp::Importer`,
etc.) with this C-only import library. Do not call mutating inline helpers that
allocate/free Assimp-owned storage across the runtime boundary.

The DLL requires the x64 Microsoft Visual C++ runtime. The `vc143` name does not
require switching the engine away from its newer v145 toolset.

## Engine integration

`NXTN::ModelLoader` exposes CPU-only `Import` and context-dependent `Upload`.
`NXTN::Scene::InstantiateModel` then creates a hierarchy-preserving instance from
the uploaded Model, without depending on ModelLoader. Imported node matrices
are decomposed through Assimp into cached local TRS, with warnings
for conversion issues. Models and scene instances share uploaded mesh ownership;
there is no mesh handle map. File materials are intentionally ignored.

See [Assets and model loading](../../docs/Assets.md) for data structures, failure
handling, lifetime requirements, and an API example. The current application
does not expose `--check-model` or the legacy importer-check method; older
instructions for those entry points no longer apply. Its startup scene is still
a demo quad.

Caching, animation playback, asynchronous loading, and automatic material
conversion are not implemented. Update the DLL, headers, hash, and precision
configuration together when upgrading Assimp. Keep `LICENSE` when redistributing
the library. General setup is covered in the [build guide](../../docs/Building.md).
