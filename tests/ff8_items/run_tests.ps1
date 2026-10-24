param(
    # Folder with the release executables named as in release_externals.h (FF8_EN.exe, FF8_FR.exe...).
    [Parameter(Mandatory = $true)][string]$ExeDirectory,
    [Parameter(Mandatory = $true)][string]$Kernel,
    [Parameter(Mandatory = $true)][string]$Mitem,
    [Parameter(Mandatory = $true)][string]$Price,
    [string]$VisualStudio,
    [string]$BuildDirectory
)
$ErrorActionPreference = 'Stop'
$repo = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if (-not $BuildDirectory) { $BuildDirectory = Join-Path $repo '.build/item-tests' }
$build = [System.IO.Path]::GetFullPath($BuildDirectory)
New-Item -ItemType Directory -Path $build -Force | Out-Null
foreach ($path in @($Kernel, $Mitem, $Price)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing input: $path" }
}
$ExeDirectory = (Resolve-Path -LiteralPath $ExeDirectory).Path
$Kernel = (Resolve-Path -LiteralPath $Kernel).Path
if (-not $VisualStudio) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $VisualStudio = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
}
$vcvars = Join-Path $VisualStudio 'VC/Auxiliary/Build/vcvarsall.bat'
if (-not (Test-Path -LiteralPath $vcvars)) { throw 'An MSVC x86 toolchain is required.' }
$fixture = Join-Path $build 'fixture'
Push-Location $repo
try {
    & py -3 tests/ff8_items/make_fixture.py $Kernel $Mitem $Price --output $fixture
    if ($LASTEXITCODE -ne 0) { throw 'Fixture generation failed.' }
    $buildScript = @"
@echo off
call "$vcvars" x86 >nul
if errorlevel 1 exit /b %ERRORLEVEL%
cl /nologo /std:c++20 /W4 /EHsc /Od /RTC1 tests\ff8_items\inventory_tests.cpp /Fo"$build\inventory_tests.obj" /Fe"$build\inventory_tests.exe"
if errorlevel 1 exit /b %ERRORLEVEL%
"$build\inventory_tests.exe"
if errorlevel 1 exit /b %ERRORLEVEL%
ml /nologo /c /coff /Fo"$build\retail_padding.obj" tests\ff8_items\retail_padding.asm
if errorlevel 1 exit /b %ERRORLEVEL%
cl /nologo /std:c++20 /W4 /Od /Gy /GS- /Oi- /Zl /D_ITERATOR_DEBUG_LEVEL=0 /D_USE_STD_VECTOR_ALGORITHMS=0 /D_CRT_SECURE_NO_WARNINGS tests\ff8_items\native_tests.cpp "$build\retail_padding.obj" /Fo"$build\native_tests.obj" /Fe"$build\native_tests.exe" /link /NODEFAULTLIB /ENTRY:audit_start /SUBSYSTEM:CONSOLE /BASE:0x400000 /DYNAMICBASE:NO /OPT:NOREF /SECTION:.text,ERW kernel32.lib libvcruntime.lib libcmt.lib
exit /b %ERRORLEVEL%
"@
    $buildScriptPath = Join-Path $build 'build-tests.cmd'
    [System.IO.File]::WriteAllText($buildScriptPath, $buildScript)
    & $buildScriptPath
    if ($LASTEXITCODE -ne 0) { throw 'Test compilation or inventory tests failed.' }
    $env:FF8_ITEM_TEST_KERNEL = Join-Path $fixture 'kernel.bin'
    $env:FF8_ITEM_TEST_VANILLA_KERNEL = $Kernel
    $releases = Select-String -Path tests/ff8_items/release_externals.h -Pattern '"(FF8_\w+\.exe)"' -AllMatches | ForEach-Object { $_.Matches.Groups[1].Value }
    $tested = 0
    foreach ($release in $releases) {
        $exe = Join-Path $ExeDirectory $release
        if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { Write-Warning "Skipping $release (not in $ExeDirectory)"; continue }
        # One process per release: the harness maps that executable at its own base.
        $env:FF8_ITEM_TEST_EXE = $exe
        $env:FF8_ITEM_TEST_RELEASE = $release
        & (Join-Path $build 'native_tests.exe')
        if ($LASTEXITCODE -ne 0) { throw "Native integration tests failed for $release." }
        $tested++
    }
    if (-not $tested) { throw "No release executable found in $ExeDirectory." }
} finally {
    Pop-Location
}
