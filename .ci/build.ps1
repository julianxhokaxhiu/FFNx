#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 Julian Xhokaxhiu                                      #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
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

$env:_RELEASE_PATH = ".dist\build\x86-${env:_RELEASE_CONFIGURATION}"
if ($env:_BUILD_BRANCH -eq "refs/heads/master" -Or $env:_BUILD_BRANCH -eq "refs/tags/canary")
{
  $env:_IS_BUILD_CANARY = "true"
}
elseif ($env:_BUILD_BRANCH -like "refs/tags/*")
{
  $env:_BUILD_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')) + ".0"
}
$env:_RELEASE_VERSION = "v${env:_BUILD_VERSION}"

Write-Output "--------------------------------------------------"
Write-Output "BUILD CONFIGURATION: $env:_RELEASE_CONFIGURATION"
Write-Output "RELEASE VERSION: $env:_RELEASE_VERSION"
Write-Output "--------------------------------------------------"

Write-Host "##vso[task.setvariable variable=_BUILD_VERSION;]${env:_BUILD_VERSION}"
Write-Host "##vso[task.setvariable variable=_RELEASE_VERSION;]${env:_RELEASE_VERSION}"
Write-Host "##vso[task.setvariable variable=_IS_BUILD_CANARY;]${env:_IS_BUILD_CANARY}"

mkdir $env:_RELEASE_PATH | Out-Null
cmake -G "Visual Studio 16 2019" -A Win32 -D_DLL_VERSION="$env:_BUILD_VERSION" -DCMAKE_BINARY_DIR="$env:_RELEASE_PATH" -S . -B $env:_RELEASE_PATH
cmake --build $env:_RELEASE_PATH --config $env:_RELEASE_CONFIGURATION

mkdir .dist\pkg\FF7_1998 | Out-Null
mkdir .dist\pkg\FF8_2000 | Out-Null
mkdir .dist\pkg\FFNx_Steam | Out-Null
Copy-Item -R "$env:_RELEASE_PATH\bin\*" .dist\pkg\FF7_1998
Copy-Item -R "$env:_RELEASE_PATH\bin\*" .dist\pkg\FF8_2000
Copy-Item -R "$env:_RELEASE_PATH\bin\*" .dist\pkg\FFNx_Steam
Remove-Item .dist\pkg\FF7_1998\FF8.reg
Remove-Item .dist\pkg\FF8_2000\FF7.reg
Remove-Item .dist\pkg\FFNx_Steam\FF7.reg
Remove-Item .dist\pkg\FFNx_Steam\FF8.reg
Move-Item .dist\pkg\FF7_1998\FF7.reg .dist\pkg\FF7_1998\FFNx.reg
Move-Item .dist\pkg\FF8_2000\FF8.reg .dist\pkg\FF8_2000\FFNx.reg
Move-Item .dist\pkg\FF8_2000\FFNx.dll .dist\pkg\FF8_2000\eax.dll
Move-Item .dist\pkg\FFNx_Steam\FFNx.dll .dist\pkg\FFNx_Steam\AF3DN.P

Compress-Archive -Path .dist\pkg\FF7_1998\* -DestinationPath ".dist\${env:_RELEASE_NAME}-FF7_1998-${env:_RELEASE_VERSION}.zip"
Compress-Archive -Path .dist\pkg\FF8_2000\* -DestinationPath ".dist\${env:_RELEASE_NAME}-FF8_2000-${env:_RELEASE_VERSION}.zip"
Compress-Archive -Path .dist\pkg\FFNx_Steam\* -DestinationPath ".dist\${env:_RELEASE_NAME}-Steam-${env:_RELEASE_VERSION}.zip"
