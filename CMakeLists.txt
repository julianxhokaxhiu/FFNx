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
#    FFNx is free software: you can redistribute it and/or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    FFNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0091 NEW)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(FATAL_ERROR "CMAKE_BUILD_TYPE must be set to continue building with cmake. \nExample: Add -DCMAKE_BUILD_TYPE=Release to your cmake command line.")
endif()
if (NOT DEFINED _DLL_VERSION OR NOT _DLL_VERSION)
  message(FATAL_ERROR "_DLL_VERSION must be set to continue building with cmake. \nExample: Add -D_DLL_VERSION=devel to your cmake command line.")
endif ()

set(VCPKG_DISABLE_COMPILER_TRACKING 1)
set(VCPKG_INSTALL_OPTIONS "--clean-after-build" "--allow-unsupported")

set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4099,4217"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
else()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
endif()

set(BGFX_SHADERC_OPENGL_FLAGS --profile 120)
set(BGFX_SHADERC_DIRECT3D_FLAGS --platform windows -p s_5_0)
set(BGFX_SHADERC_VULKAN_FLAGS --platform windows --profile spirv)

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
  list(APPEND BGFX_SHADERC_DIRECT3D_FLAGS -O3)
else()
  list(APPEND BGFX_SHADERC_DIRECT3D_FLAGS --debug -O0)
  list(APPEND BGFX_SHADERC_VULKAN_FLAGS --debug -O0)
endif()

option(FORCEHEAP "Force all allocation to our heap" OFF)
if(FORCEHEAP)
  add_definitions(-DNO_EXT_HEAP)
endif()

option(TRACEHEAP "Trace and keep count of every allocation made by this program" OFF)
if(TRACEHEAP)
  add_definitions(-DHEAP_DEBUG)
endif()

option(PROFILING "Enable Profiling" OFF)
if(PROFILING)
  add_definitions(-DPROFILE)
endif()

project(FFNx)

find_package(ZLIB REQUIRED)
find_package(BX REQUIRED)
find_package(BIMG REQUIRED)
find_package(BGFX REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(MPG123 REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(VGMSTREAM REQUIRED)
find_package(STACKWALKER REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(directxtex CONFIG REQUIRED)
find_package(mimalloc CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SOLOUD REQUIRED)
find_package(OPENPSF REQUIRED)
find_package(STEAMWORKSSDK CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)
find_package(LZ4 REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)
find_package(lfreist-hwinfo CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

set(RELEASE_NAME "FFNx")

if(_DLL_VERSION STREQUAL "devel" OR _DLL_VERSION MATCHES "-")
  set(_DLL_RCVERSION "0,0,0,0")
  set(_DLL_RCSTRVERSION "0.0.0.0")
  set(PATCH_COLLECT_DUPLICATES 1)
else()
  string(REPLACE "." "," _DLL_RCVERSION ${_DLL_VERSION})
  set(_DLL_RCSTRVERSION ${_DLL_VERSION})
endif()

# Choco driver
set(CHOCO_RELEASE_NAME "FFNx-Choco")
file(GLOB_RECURSE choco_source_files "${CMAKE_SOURCE_DIR}/choco/*.cpp")
add_library(${CHOCO_RELEASE_NAME} SHARED ${choco_source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
target_include_directories(
  ${CHOCO_RELEASE_NAME}
  PRIVATE "${CMAKE_SOURCE_DIR}/choco"
)
target_link_libraries(
  ${CHOCO_RELEASE_NAME}
  shlwapi
)
target_compile_options(
  ${CHOCO_RELEASE_NAME}
  PRIVATE /DVERSION="${_DLL_VERSION}"
  PRIVATE /D_CRT_SECURE_NO_WARNINGS
  PRIVATE /DNOMINMAX
  PRIVATE /Zc:strictStrings-
  PRIVATE /Zc:__cplusplus
  PRIVATE /Zc:preprocessor
  PRIVATE /Qpar
  PRIVATE /MP
)
target_compile_features(${CHOCO_RELEASE_NAME}
  PRIVATE cxx_std_20
)
target_link_options(${CHOCO_RELEASE_NAME} PRIVATE /PDBALTPATH:${CHOCO_RELEASE_NAME}.pdb PRIVATE /DEF:${CMAKE_SOURCE_DIR}/misc/${CHOCO_RELEASE_NAME}.def)

# Include all the source code files
file(GLOB_RECURSE source_files "${CMAKE_SOURCE_DIR}/src/*.cpp")

configure_file(misc/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

cmrc_add_resource_library(
  ${RELEASE_NAME}-resources
  ALIAS ${RELEASE_NAME}::rc
  NAMESPACE ${RELEASE_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/.logo/logo_nobg.png
)

add_library(${RELEASE_NAME} SHARED ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
add_dependencies(${RELEASE_NAME} ${CHOCO_RELEASE_NAME})
target_include_directories(
  ${RELEASE_NAME}
  PRIVATE "${CMAKE_SOURCE_DIR}/src"
  PRIVATE ${FFMPEG_INCLUDE_DIRS}
  PRIVATE ${BX_INCLUDE_DIRS}
  PRIVATE ${BIMG_INCLUDE_DIRS}
  PRIVATE ${BGFX_INCLUDE_DIRS}
  PRIVATE ${OPENPSF_INCLUDE_DIRS}
  PRIVATE ${SOLOUD_INCLUDE_DIRS}
  PRIVATE ${STACKWALKER_INCLUDE_DIRS}
  PRIVATE ${VGMSTREAM_INCLUDE_DIRS}
)
target_link_directories(
  ${RELEASE_NAME}
  PRIVATE ${FFMPEG_LIBRARY_DIRS}
)
target_link_libraries(
  ${RELEASE_NAME}
  ${RELEASE_NAME}::rc
  comctl32
  wintrust
  dbghelp
  shlwapi
  psapi
  dwmapi
  winmm
  dinput8
  dxguid
  XINPUT9_1_0
  strmiids
  ZLIB::ZLIB
  PNG::PNG
  Microsoft::DirectXTex
  mimalloc-static
  imgui::imgui
  pugixml::pugixml
  PkgConfig::tomlplusplus
  STEAMWORKSSDK::STEAMWORKSSDK
  xxHash::xxhash
  lz4::lz4
  Vorbis::vorbisfile
  Vorbis::vorbis
  MPG123::libmpg123
  lfreist-hwinfo::hwinfo
  unofficial::stackwalker::StackWalker
  ${BX_LIBRARIES}
  ${BIMG_LIBRARIES}
  ${BGFX_LIBRARIES}
  ${FFMPEG_LIBRARIES}
  ${OPENPSF_LIBRARIES}
  ${SOLOUD_LIBRARIES}
  ${VGMSTREAM_LIBRARIES}
)
target_compile_options(
  ${RELEASE_NAME}
  PRIVATE /DBX_CONFIG_DEBUG=1
  PRIVATE /DDIRECTINPUT_VERSION=0x0800
  PRIVATE /DVERSION="${_DLL_VERSION}"
  PRIVATE /D_CRT_SECURE_NO_WARNINGS
  PRIVATE /DNOMINMAX
  PRIVATE /Zc:strictStrings-
  PRIVATE /Zc:__cplusplus
  PRIVATE /Zc:preprocessor
  PRIVATE /Qpar
  PRIVATE /MP
)
if(PATCH_COLLECT_DUPLICATES)
  target_compile_definitions(${RELEASE_NAME}
    PRIVATE PATCH_COLLECT_DUPLICATES
  )
endif()
target_compile_features(${RELEASE_NAME}
  PRIVATE cxx_std_20
)
target_link_options(
  ${RELEASE_NAME}
  PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb
  PRIVATE /DEF:${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.def
  PRIVATE /MANIFESTDEPENDENCY:"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'"
)

# SHADER COMPILATION
set(FFNX_SHADERS "FFNx" "FFNx.lighting" "FFNx.shadowmap" "FFNx.field.shadow" "FFNx.overlay" "FFNx.post" "FFNx.blit")
foreach(FFNX_SHADER IN LISTS FFNX_SHADERS)
  foreach(BGFX_VARYING flat smooth)
    add_custom_command(
      TARGET ${RELEASE_NAME}
      POST_BUILD
      COMMAND echo Building ${FFNX_SHADER}.${BGFX_VARYING} vert/frag shaders...
      # ensure bin directory exists
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
      # OpenGL
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.frag -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.gl.frag --type f --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_OPENGL_FLAGS}
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.vert -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.gl.vert --type v --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_OPENGL_FLAGS}
      # Vulkan
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.frag -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.vk.frag --type f --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_VULKAN_FLAGS}
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.vert -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.vk.vert --type v --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_VULKAN_FLAGS}
      # Direct3D 11
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.frag -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.d3d11.frag --type f --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_DIRECT3D_FLAGS}
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.vert -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.d3d11.vert --type v --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_DIRECT3D_FLAGS}
      # Direct3D 12
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.frag -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.d3d12.frag --type f --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_DIRECT3D_FLAGS}
      COMMAND
        ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${FFNX_SHADER}.vert -o
        ${CMAKE_BINARY_DIR}/bin/shaders/${FFNX_SHADER}.${BGFX_VARYING}.d3d12.vert --type v --varyingdef
        ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc ${BGFX_SHADERC_DIRECT3D_FLAGS})
  endforeach()
endforeach()

# List .gitkeep files to remove them on INSTALL
FILE(GLOB_RECURSE HEXT_GIT_KEEP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/misc/ ${CMAKE_CURRENT_SOURCE_DIR}/misc/hext/ff8/**/.gitkeep)
LIST(TRANSFORM HEXT_GIT_KEEP_FILES PREPEND ${CMAKE_BINARY_DIR}/bin/)

# INSTALL
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  COMMAND echo Preparing ${RELEASE_NAME} ${_DLL_VERSION} release...
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
  # License
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.TXT
          ${CMAKE_BINARY_DIR}/bin
  # .dll
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.dll
          ${CMAKE_BINARY_DIR}/bin
  # choco .dll
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CHOCO_RELEASE_NAME}.dll
          ${CMAKE_BINARY_DIR}/bin
  # rename choco .dll to AF4DN.P
  COMMAND ${CMAKE_COMMAND} -E rename
          ${CMAKE_BINARY_DIR}/bin/${CHOCO_RELEASE_NAME}.dll
          ${CMAKE_BINARY_DIR}/bin/AF4DN.P
  # steam_api.dll
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/steamworkssdk/steam_api.dll
          ${CMAKE_BINARY_DIR}/bin
  # .pdb
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.pdb
          ${CMAKE_BINARY_DIR}/bin
  # .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.toml
          ${CMAKE_BINARY_DIR}/bin
  # FF7.reg
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/FF7.reg
          ${CMAKE_BINARY_DIR}/bin
  # FF8.reg
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/FF8.reg
          ${CMAKE_BINARY_DIR}/bin
  # hext
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/hext
          ${CMAKE_BINARY_DIR}/bin/hext
  COMMAND ${CMAKE_COMMAND} -E rm ${HEXT_GIT_KEEP_FILES}

  # ambient .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.ambient.toml
          ${CMAKE_BINARY_DIR}/bin/ambient/config.toml
  # lighting .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.toml
          ${CMAKE_BINARY_DIR}/bin/lighting/config.toml
  # SFX .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.SFX.toml
          ${CMAKE_BINARY_DIR}/bin/sfx/config.toml
  # Music .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.music.toml
          ${CMAKE_BINARY_DIR}/bin/music/vgmstream/config.toml
  # Voice .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.voice.toml
          ${CMAKE_BINARY_DIR}/bin/voice/config.toml
  # Vibrate field .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.vibrate.ff8.field.toml
          ${CMAKE_BINARY_DIR}/bin/vibrate/ff8/field.toml
  # Vibrate world .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.vibrate.ff8.world.toml
          ${CMAKE_BINARY_DIR}/bin/vibrate/ff8/world.toml
  # Vibrate battle .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.vibrate.ff8.battle.toml
          ${CMAKE_BINARY_DIR}/bin/vibrate/ff8/battle.toml
  # time cycle .toml
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/${RELEASE_NAME}.time.toml
          ${CMAKE_BINARY_DIR}/bin/time/config.toml
  # Gamut LUTS
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_ntscj_to_srgb.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_smptec_to_srgb.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_ebu_to_srgb.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_inverse_ntscj_to_srgb.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_inverse_ntscj_to_smptec.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_inverse_ntscj_to_ebu.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_srgb_to_ntscj.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_smptec_to_ntscj.png
          ${CMAKE_BINARY_DIR}/bin/shaders
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/misc/glut_ebu_to_ntscj.png
          ${CMAKE_BINARY_DIR}/bin/shaders

)

# CPU INFO
add_custom_command(
  TARGET ${CHOCO_RELEASE_NAME}
  PRE_BUILD
  # Get CPU info
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/cpuinfo/cpu-info
  # Get CPU ISA supported instructions
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/cpuinfo/isa-info
)

# Copy FFNx release to FF7 1998 game path if installed
cmake_host_system_information(RESULT FF7_1998_GAME_PATH QUERY WINDOWS_REGISTRY "HKLM/SOFTWARE/Square Soft, Inc./Final Fantasy VII" VALUE "AppPath" ERROR_VARIABLE FF7_KEY_QUERY_RES)
if("${FF7_KEY_QUERY_RES}" STREQUAL "")
  cmake_path(CONVERT "${FF7_1998_GAME_PATH}" TO_CMAKE_PATH_LIST FF7_1998_GAME_PATH)
  # Ensure FFNx.toml exists so the copy on build does not error on fresh installs
  if(NOT EXISTS "${FF7_1998_GAME_PATH}/FFNx.toml")
    # Copy file if it doesn't exist
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/misc/FFNx.toml" DESTINATION "${FF7_1998_GAME_PATH}/")
  endif()
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    COMMAND echo Copying ${RELEASE_NAME} ${_DLL_VERSION} release to ${FF7_1998_GAME_PATH}...
    # Preserve the current FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF7_1998_GAME_PATH}/FFNx.toml
            ${FF7_1998_GAME_PATH}/FFNx.toml.bak
    # Copy all dist files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_BINARY_DIR}/bin
            ${FF7_1998_GAME_PATH}
    # Delete the new copied FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF7_1998_GAME_PATH}/FFNx.toml
    # Bring back the existing FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF7_1998_GAME_PATH}/FFNx.toml.bak
            ${FF7_1998_GAME_PATH}/FFNx.toml
  )
endif()

# Copy FFNx release to FF8 2000 game path if installed
cmake_host_system_information(RESULT FF8_2000_GAME_PATH QUERY WINDOWS_REGISTRY "HKLM/SOFTWARE/Square Soft, Inc/FINAL FANTASY VIII/1.00" VALUE "AppPath" ERROR_VARIABLE FF8_KEY_QUERY_RES)
if("${FF8_KEY_QUERY_RES}" STREQUAL "")
  cmake_path(CONVERT "${FF8_2000_GAME_PATH}" TO_CMAKE_PATH_LIST FF8_2000_GAME_PATH)
  # Ensure FFNx.toml exists so the copy on build does not error on fresh installs
  if(NOT EXISTS "${FF8_2000_GAME_PATH}/FFNx.toml")
    # Copy file if it doesn't exist
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/misc/FFNx.toml" DESTINATION "${FF8_2000_GAME_PATH}/")
  endif()
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    COMMAND echo Copying ${RELEASE_NAME} ${_DLL_VERSION} release to ${FF8_2000_GAME_PATH}...
    # Preserve the current FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF8_2000_GAME_PATH}/FFNx.toml
            ${FF8_2000_GAME_PATH}/FFNx.toml.bak
    # Copy all dist files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_BINARY_DIR}/bin
            ${FF8_2000_GAME_PATH}
    # Delete previous eax.dll
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF8_2000_GAME_PATH}/eax.dll
    # Rename FFNx.dll to eax.dll
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF8_2000_GAME_PATH}/FFNx.dll
            ${FF8_2000_GAME_PATH}/eax.dll
    # Delete the new copied FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF8_2000_GAME_PATH}/FFNx.toml
    # Bring back the existing FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF8_2000_GAME_PATH}/FFNx.toml.bak
            ${FF8_2000_GAME_PATH}/FFNx.toml
  )
endif()

# Copy FFNx release to FF7 Steam game path if installed
execute_process(
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/FindSteamGamePath 39140
  OUTPUT_VARIABLE FF7_STEAM_GAME_PATH
  ERROR_VARIABLE FF7_STEAM_ERROR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT "${FF7_STEAM_GAME_PATH}" STREQUAL "")
  cmake_path(CONVERT "${FF7_STEAM_GAME_PATH}" TO_CMAKE_PATH_LIST FF7_STEAM_GAME_PATH)
  # Ensure FFNx.toml exists so the copy on build does not error on fresh installs
  if(NOT EXISTS "${FF7_STEAM_GAME_PATH}/FFNx.toml")
    # Copy file if it doesn't exist
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/misc/FFNx.toml" DESTINATION "${FF7_STEAM_GAME_PATH}/")
  endif()
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    COMMAND echo Copying ${RELEASE_NAME} ${_DLL_VERSION} release to ${FF7_STEAM_GAME_PATH}...
    # Preserve the current FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF7_STEAM_GAME_PATH}/FFNx.toml
            ${FF7_STEAM_GAME_PATH}/FFNx.toml.bak
    # Copy all dist files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_BINARY_DIR}/bin
            ${FF7_STEAM_GAME_PATH}
    # Delete previous AF3DN.P
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF7_STEAM_GAME_PATH}/AF3DN.P
    # Rename FFNx.dll to AF3DN.P
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF7_STEAM_GAME_PATH}/FFNx.dll
            ${FF7_STEAM_GAME_PATH}/AF3DN.P
    # Delete the new copied FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF7_STEAM_GAME_PATH}/FFNx.toml
    # Bring back the existing FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF7_STEAM_GAME_PATH}/FFNx.toml.bak
            ${FF7_STEAM_GAME_PATH}/FFNx.toml
  )
endif()

# Copy FFNx release to FF7 Steam game path if installed
execute_process(
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/utils/FindSteamGamePath 39150
  OUTPUT_VARIABLE FF8_STEAM_GAME_PATH
  ERROR_VARIABLE FF8_STEAM_ERROR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT "${FF8_STEAM_GAME_PATH}" STREQUAL "")
  cmake_path(CONVERT "${FF8_STEAM_GAME_PATH}" TO_CMAKE_PATH_LIST FF8_STEAM_GAME_PATH)
  # Ensure FFNx.toml exists so the copy on build does not error on fresh installs
  if(NOT EXISTS "${FF8_STEAM_GAME_PATH}/FFNx.toml")
    # Copy file if it doesn't exist
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/misc/FFNx.toml" DESTINATION "${FF8_STEAM_GAME_PATH}/")
  endif()
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    COMMAND echo Copying ${RELEASE_NAME} ${_DLL_VERSION} release to ${FF8_STEAM_GAME_PATH}...
    # Preserve the current FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF8_STEAM_GAME_PATH}/FFNx.toml
            ${FF8_STEAM_GAME_PATH}/FFNx.toml.bak
    # Copy all dist files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_BINARY_DIR}/bin
            ${FF8_STEAM_GAME_PATH}
    # Delete previous AF3DN.P
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF8_STEAM_GAME_PATH}/AF3DN.P
    # Rename FFNx.dll to AF3DN.P
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF8_STEAM_GAME_PATH}/FFNx.dll
            ${FF8_STEAM_GAME_PATH}/AF3DN.P
    # Delete the new copied FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E remove
            ${FF8_STEAM_GAME_PATH}/FFNx.toml
    # Bring back the existing FFNx.toml
    COMMAND ${CMAKE_COMMAND} -E rename
            ${FF8_STEAM_GAME_PATH}/FFNx.toml.bak
            ${FF8_STEAM_GAME_PATH}/FFNx.toml
  )
endif()
