param(
    [Parameter(Mandatory = $true)][string]$MsvcBin,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$assimpDll = Join-Path $PSScriptRoot 'assimp-vc143-mt.dll'
$assimpDumpbin = Join-Path $MsvcBin 'dumpbin.exe'
$assimpLibrarian = Join-Path $MsvcBin 'lib.exe'

# Headers and generated configuration are pinned to this official x64 release.
$assimpHasher = [System.Security.Cryptography.SHA256]::Create()
try {
    $assimpHash = [System.BitConverter]::ToString($assimpHasher.ComputeHash([System.IO.File]::ReadAllBytes($assimpDll))).Replace('-', '')
} finally {
    $assimpHasher.Dispose()
}
if ($assimpHash -ne
    '253E2B63978A3AD42CC32CE7659DC078E9CCC64392DFA42E7AE3D08322922EC1') {
    throw 'Assimp DLL differs from the pinned 6.0.5 x64 release. Update headers and configuration together.'
}

New-Item -ItemType Directory -Path (Join-Path $OutputDirectory 'include/assimp') -Force | Out-Null
$assimpConfig = [System.IO.File]::ReadAllText((Join-Path $PSScriptRoot 'include/assimp/config.h.in'))
$assimpConfig = $assimpConfig.Replace('#cmakedefine ASSIMP_DOUBLE_PRECISION 1', '/* #undef ASSIMP_DOUBLE_PRECISION */')
$assimpConfigPath = Join-Path $OutputDirectory 'include/assimp/config.h'
if (!(Test-Path -LiteralPath $assimpConfigPath) -or
    [System.IO.File]::ReadAllText($assimpConfigPath) -ne $assimpConfig) {
    [System.IO.File]::WriteAllText($assimpConfigPath, $assimpConfig)
}

$assimpExports = & $assimpDumpbin /nologo /exports $assimpDll
if ($LASTEXITCODE -ne 0) { throw 'Could not read Assimp DLL exports.' }
$assimpSymbols = @($assimpExports | ForEach-Object {
    if ($_ -match '^\s+\d+\s+[0-9A-F]+\s+[0-9A-F]+\s+(ai[A-Za-z0-9_]+)\s*$') {
        $Matches[1]
    }
} | Sort-Object -Unique)
foreach ($assimpRequired in @('aiImportFile', 'aiImportFileFromMemory', 'aiReleaseImport', 'aiGetCompileFlags')) {
    if ($assimpRequired -notin $assimpSymbols) { throw "Missing Assimp export: $assimpRequired" }
}

# Only the C API is exposed: the release DLL's C++ API is not a Debug CRT boundary.
$assimpDefinition = "LIBRARY assimp-vc143-mt.dll`r`nEXPORTS`r`n" + ($assimpSymbols -join "`r`n") + "`r`n"
$assimpDefinitionPath = Join-Path $OutputDirectory 'assimp-c-api.def'
$assimpLibraryPath = Join-Path $OutputDirectory 'assimp-c-api.lib'
if (!(Test-Path -LiteralPath $assimpDefinitionPath) -or
    [System.IO.File]::ReadAllText($assimpDefinitionPath) -ne $assimpDefinition -or
    !(Test-Path -LiteralPath $assimpLibraryPath)) {
    [System.IO.File]::WriteAllText($assimpDefinitionPath, $assimpDefinition)
    & $assimpLibrarian /nologo /machine:x64 "/def:$assimpDefinitionPath" "/out:$assimpLibraryPath"
    if ($LASTEXITCODE -ne 0) { throw 'Could not generate the Assimp C API import library.' }
}
