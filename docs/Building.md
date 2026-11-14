# Build and run

[Wiki](README.md) · [Architecture](Architecture.md) · [Assimp integration](../external/Assimp/README.md)

These instructions describe the repository's current Windows setup, not a
cross-platform build. Build settings come from [premake5.lua](../premake5.lua).

## Requirements

- Windows x64, a driver supporting OpenGL 4.5 core, and the project dependencies.
- Visual Studio 2026 C++ tools, including the v145 toolset and a Windows SDK.
- A Premake executable supporting the `vs2026` action. The executable inspected
  with this working tree reports `5.0.0-beta8`.
- PowerShell, used by the Assimp prebuild step.

The current [generation script](../Generate%20Project.bat) expects Premake at
`../premake/premake5.exe` relative to the repository root. That executable is
outside this repository; a fresh checkout does not supply it.

## Dependencies

| Dependency | Location / integration |
| --- | --- |
| GLAD | `external/GLAD`; built as a project dependency |
| GLFW | Headers in `external/GLFW/include`, prebuilt `external/GLFW/lib/glfw3_mt.lib` |
| Dear ImGui | `external/imgui`; Git submodule with the project's Premake setup |
| stb_image | `external/stb_image`; compiled with engine sources |
| Assimp | `external/Assimp`; pinned 6.0.5 x64 DLL, matching headers, generated C import library |

For a fresh clone, initialize the configured submodule from the repository root:

```powershell
git submodule update --init --recursive
```

For an existing checkout with local dependency changes, inspect those changes
before updating the submodule. See [.gitmodules](../.gitmodules) for its origin.

## Generate and build

Run from the repository root:

```powershell
& '..\premake\premake5.exe' vs2026
```

Alternatively run `Generate Project.bat` there. Open the generated
`InkStone.slnx` in Visual Studio 2026, select x64 and Debug or Release, and build
the `engine` startup project. An older `InkStone.sln` also exists; do not assume
it represents the latest generated project set.

From a Visual Studio 2026 Developer PowerShell, an equivalent build is:

```powershell
msbuild .\InkStone.slnx /m /p:Configuration=Debug /p:Platform=x64
```

Premake selects C++23. The inspected generated project uses v145 and
`stdcpp23`. Debug uses the static debug CRT (`/MTd`); Release uses the static
release CRT (`/MT`). Generated project edits are not the source of truth:
change Premake and regenerate when changing project configuration or adding
source files covered by its wildcards.

Outputs are under `bin/x86_64/{Debug,Release}/engine/`; intermediate files are
under `bin-obj/x86_64/{Debug,Release}/engine/`.

## Assimp prebuild and deployment

Before engine compilation, Premake's generated prebuild command runs
[Prepare.ps1](../external/Assimp/Prepare.ps1) with the active MSVC tools. It
checks the pinned DLL hash, generates the single-precision configuration header,
and creates `assimp-c-api.lib` from the DLL's C exports using `dumpbin.exe` and
`lib.exe`. This does not compile Assimp or download anything.

The postbuild step copies `assimp-vc143-mt.dll` next to `engine.exe`. The PDB is
optional debug information, not a link or runtime requirement. The DLL filename
does not select the engine's platform toolset. The integration deliberately
uses Assimp's C API; do not switch to `Assimp::Importer` while retaining the
C-only import library.

Keep the matching headers, DLL, configuration, and pinned hash together when
upgrading. Preserve the [upstream license](../external/Assimp/LICENSE) when
redistributing the dependency. Additional provenance and runtime information
are in the [Assimp README](../external/Assimp/README.md).

## Run with the right working directory

Application currently opens assets via `../Asset/...`, so its working directory
must be the repository's `engine` directory. In Visual Studio, use `engine` as
the startup project and verify its debugger working directory is `$(ProjectDir)`.

From the repository root in PowerShell:

```powershell
Push-Location .\engine
try {
    & '..\bin\x86_64\Debug\engine\engine.exe'
}
finally {
    Pop-Location
}
```

The current demo loads the split Texture shader and a UV texture, then displays
a hard-coded quad with viewport and hierarchy UI. There is no current
`--check-model` mode or standalone importer-check method. To load a model, call
`ModelLoader::Import`, `ModelLoader::Upload`, and `Scene::InstantiateModel` from
application code; see the [three-stage API](Assets.md#three-separate-operations).

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Missing v143 build tools | Regenerate with the configured `vs2026` action and open the new solution; inspect which project still requests v143. Do not edit the Assimp DLL's name to change a toolset. |
| Premake does not recognize `vs2026` | The executable is older/different from this setup. Use a Premake build with that action; changing only the IDE does not change the generator. |
| Missing `assimp/config.h` or `assimp-c-api.lib` | Check the prebuild result and active MSVC `dumpbin`/`lib` paths. Generated files belong in the configuration's intermediate directory. |
| Assimp hash mismatch | The DLL does not match the pinned integration; update the dependency as a set instead of bypassing verification. |
| DLL not found at launch | Verify the postbuild copy and the dependency's required x64 Visual C++ runtime. |
| Shader/image cannot open, or startup fails after shader loading | Verify working directory and Asset files. Demo Material construction does not safely handle an invalid shader yet. |
| OpenGL context creation/load failure | Check the GPU driver and OpenGL 4.5 support; no alternate renderer is available. |

