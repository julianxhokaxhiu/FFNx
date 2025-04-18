#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
#    Copyright (C) 2025 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of FFNx                                                #
#                                                                             #
#    FFNx is free software: you can redistribute it and\or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    FFNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

Set-StrictMode -Version Latest

if ($env:_BUILD_BRANCH -eq "refs/heads/master" -Or $env:_BUILD_BRANCH -eq "refs/tags/canary")
{
  $env:_IS_BUILD_CANARY = "true"
  $env:_IS_GITHUB_RELEASE = "true"
}
elseif ($env:_BUILD_BRANCH -like "refs/tags/*")
{
  $env:_CHANGELOG_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')).Replace('.','')
  $env:_BUILD_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')) + ".0"
  $env:_IS_GITHUB_RELEASE = "true"
}
$env:_RELEASE_VERSION = "v${env:_BUILD_VERSION}"

$vcpkgRoot = ".\vcpkg"
$vcpkgBaseline = [string](jq --arg baseline "builtin-baseline" -r '.[$baseline]' vcpkg.json)
$vcpkgOriginUrl = &"git" -C $vcpkgRoot remote get-url origin
$vcpkgTagName = &"git" -C $vcpkgRoot describe --exact-match --tags

$releasePath = [string](jq -r '.configurePresets[0].binaryDir' CMakePresets.json).Replace('${sourceDir}/', '')

Write-Output "--------------------------------------------------"
Write-Output "BUILD CONFIGURATION: $env:_RELEASE_CONFIGURATION"
Write-Output "RELEASE VERSION: $env:_RELEASE_VERSION"
Write-Output "VCPKG ORIGIN: $vcpkgOriginUrl"
Write-Output "VCPKG TAG: $vcpkgTagName"
Write-Output "VCPKG BASELINE: $vcpkgBaseline"
Write-Output "--------------------------------------------------"

Write-Output "_BUILD_VERSION=${env:_BUILD_VERSION}" >> ${env:GITHUB_ENV}
Write-Output "_RELEASE_VERSION=${env:_RELEASE_VERSION}" >> ${env:GITHUB_ENV}
Write-Output "_IS_BUILD_CANARY=${env:_IS_BUILD_CANARY}" >> ${env:GITHUB_ENV}
Write-Output "_IS_GITHUB_RELEASE=${env:_IS_GITHUB_RELEASE}" >> ${env:GITHUB_ENV}
Write-Output "_CHANGELOG_VERSION=${env:_CHANGELOG_VERSION}" >> ${env:GITHUB_ENV}

# Install CMake
Write-Output "Installing cmake v${env:_WINGET_CMAKE}..."
winget install Kitware.CMake --version ${env:_WINGET_CMAKE} --silent --uninstall-previous --accept-source-agreements --accept-package-agreements --disable-interactivity --force | out-null
cmake --version

# Install Visual Studio Enterprise
Write-Output "Installing VisualStudio 2022 Enterprise v${env:_WINGET_VS2022}..."
winget install Microsoft.VisualStudio.2022.Enterprise --version ${env:_WINGET_VS2022} --silent --accept-source-agreements --accept-package-agreements --disable-interactivity --force | out-null

# Load vcvarsall environment for x86
$vcvarspath = &"${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -prerelease -latest -property InstallationPath
cmd.exe /c "call `"$vcvarspath\VC\Auxiliary\Build\vcvarsall.bat`" x86 && set > %temp%\vcvars.txt"
Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
  if ($_ -match "^(.*?)=(.*)$") {
    Set-Content "env:\$($matches[1])" $matches[2]
  }
}

# Unset VCPKG_ROOT if set
[Environment]::SetEnvironmentVariable('VCPKG_ROOT','')

# Add Github Packages registry
nuget sources add -Name github -Source "https://nuget.pkg.github.com/julianxhokaxhiu/index.json" -Username ${env:GITHUB_REPOSITORY_OWNER} -Password ${env:GITHUB_PACKAGES_PAT} -StorePasswordInClearText
nuget setApiKey ${env:GITHUB_PACKAGES_PAT} -Source "https://nuget.pkg.github.com/julianxhokaxhiu/index.json"
nuget sources list

# Vcpkg setup
cmd.exe /c "call $vcpkgRoot\bootstrap-vcpkg.bat"

vcpkg integrate install

# Start the build
cmake --preset "${env:_RELEASE_CONFIGURATION}" -D_DLL_VERSION="$env:_BUILD_VERSION"
cmake --build --preset "${env:_RELEASE_CONFIGURATION}"

# Start the packaging
mkdir .dist\pkg\FF7_1998 | Out-Null
mkdir .dist\pkg\FF8_2000 | Out-Null
mkdir .dist\pkg\FFNx_Steam | Out-Null
Copy-Item -R "$releasePath\bin\*" .dist\pkg\FF7_1998
Copy-Item -R "$releasePath\bin\*" .dist\pkg\FF8_2000
Copy-Item -R "$releasePath\bin\*" .dist\pkg\FFNx_Steam
Remove-Item .dist\pkg\FF7_1998\FF8.reg
Remove-Item .dist\pkg\FF8_2000\FF7.reg
Remove-Item .dist\pkg\FFNx_Steam\FF7.reg
Remove-Item .dist\pkg\FFNx_Steam\FF8.reg
Remove-Item .dist\pkg\FF7_1998\AF4DN.P
Remove-Item .dist\pkg\FF8_2000\AF4DN.P
Move-Item .dist\pkg\FF7_1998\FF7.reg .dist\pkg\FF7_1998\FFNx.reg
Move-Item .dist\pkg\FF8_2000\FF8.reg .dist\pkg\FF8_2000\FFNx.reg
Move-Item .dist\pkg\FF8_2000\FFNx.dll .dist\pkg\FF8_2000\eax.dll
Move-Item .dist\pkg\FFNx_Steam\FFNx.dll .dist\pkg\FFNx_Steam\AF3DN.P

7z a ".\.dist\${env:_RELEASE_NAME}-FF7_1998-${env:_RELEASE_VERSION}.zip" ".\.dist\pkg\FF7_1998\*"
7z a ".\.dist\${env:_RELEASE_NAME}-FF8_2000-${env:_RELEASE_VERSION}.zip" ".\.dist\pkg\FF8_2000\*"
7z a ".\.dist\${env:_RELEASE_NAME}-Steam-${env:_RELEASE_VERSION}.zip" ".\.dist\pkg\FFNx_Steam\*"

Remove-Item -Recurse -Force .dist\pkg
