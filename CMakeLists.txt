#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
#    Copyright (C) 2022 Julian Xhokaxhiu                                      #
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

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(FATAL_ERROR "CMAKE_BUILD_TYPE must be set to continue building with cmake. \nExample: Add -DCMAKE_BUILD_TYPE=Release to your cmake command line.")
endif()
if (NOT DEFINED _DLL_VERSION OR NOT _DLL_VERSION)
  message(FATAL_ERROR "_DLL_VERSION must be set to continue building with cmake. \nExample: Add -D_DLL_VERSION=devel to your cmake command line.")
endif ()

set(VCPKG_DISABLE_COMPILER_TRACKING 1)
set(VCPKG_INSTALL_OPTIONS "--clean-after-build" "--x-use-aria2")
set(VCPKG_TARGET_TRIPLET x86-windows-static)

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
find_package(VGMSTREAM REQUIRED)
find_package(STACKWALKER REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(libpng CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SOLOUD REQUIRED)
find_package(OPENPSF REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(STEAMWORKSSDK CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)

set(RELEASE_NAME "FFNx")

if(_DLL_VERSION STREQUAL "devel" OR _DLL_VERSION MATCHES "-")
  set(_DLL_RCVERSION "0,0,0,0")
  set(_DLL_RCSTRVERSION "0.0.0.0")
  set(PATCH_COLLECT_DUPLICATES 1)
else()
  string(REPLACE "." "," _DLL_RCVERSION ${_DLL_VERSION})
  set(_DLL_RCSTRVERSION ${_DLL_VERSION})
endif()

# Include all the source code files
file(GLOB_RECURSE source_files "${CMAKE_SOURCE_DIR}/src/*.cpp")

configure_file(misc/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

add_library(${RELEASE_NAME} SHARED ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
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
target_link_libraries(
  ${RELEASE_NAME}
  dbghelp
  shlwapi
  psapi
  winmm
  dinput8
  dxguid
  XINPUT9_1_0
  strmiids
  ZLIB::ZLIB
  png_static
  imgui::imgui
  pugixml::pugixml
  tomlplusplus::tomlplusplus
  STEAMWORKSSDK::STEAMWORKSSDK
  xxHash::xxhash
  ${BX_LIBRARIES}
  ${BIMG_LIBRARIES}
  ${BGFX_LIBRARIES}
  ${FFMPEG_LIBRARIES}
  ${OPENPSF_LIBRARIES}
  ${SOLOUD_LIBRARIES}
  ${STACKWALKER_LIBRARIES}
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
  PRIVATE /Qpar
)
if(PATCH_COLLECT_DUPLICATES)
  target_compile_definitions(${RELEASE_NAME}
    PRIVATE PATCH_COLLECT_DUPLICATES
  )
endif()
target_compile_features(${RELEASE_NAME}
  PRIVATE cxx_std_20
)
target_link_options(${RELEASE_NAME} PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb PRIVATE /DEF:${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.def)

# SHADER COMPILATION

# GAME SHADERS
foreach(BGFX_VARYING flat smooth)
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    # ensure bin directory exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
    # OpenGL
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.gl.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --profile 120
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.gl.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --profile 120
    # Vulkan
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.vk.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows --profile spirv
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.vk.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows --profile spirv
    # Direct3D 9
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.d3d9.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_3_0 -O 3
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.d3d9.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_3_0 -O 3
    # Direct3D 11
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.d3d11.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.d3d11.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_5_0 -O 3
    # Direct3D 12
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.d3d12.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o
      ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.${BGFX_VARYING}.d3d12.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_5_0 -O 3)
endforeach()

# LIGHTING SHADERS
foreach(BGFX_VARYING flat smooth)
  add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    # ensure bin directory exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
    # OpenGL
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.frag
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.gl.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --profile 120
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.vert
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.gl.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --profile 120
    # Vulkan
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.frag
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.vk.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows --profile spirv
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.vert
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.vk.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows --profile spirv
    # Direct3D 9
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.frag
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.d3d9.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_3_0 -O 3
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.vert
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.d3d9.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_3_0 -O 3
    # Direct3D 11
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.frag
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.d3d11.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.vert
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.d3d11.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_5_0 -O 3
    # Direct3D 12
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.frag
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.d3d12.frag --type f --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND
      ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.lighting.vert
      -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.lighting.${BGFX_VARYING}.d3d12.vert --type v --varyingdef
      ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_5_0 -O 3)
endforeach()

# SHADOWMAP SHADERS
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
  # OpenGL
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --profile 120
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --profile 120
  # Vulkan
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows --profile spirv
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows --profile spirv
  # Direct3D 9
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.d3d9.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_3_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.d3d9.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_3_0 -O 3
  # Direct3D 11
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_5_0 -O 3
  # Direct3D 12
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.shadowmap.vert
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.shadowmap.d3d12.vert --type v --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3)

# FIELD SHADOW SHADERS
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
  # OpenGL
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.frag
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.gl.frag --type f --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --profile 120
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.vert
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.gl.vert --type v --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --profile 120
  # Vulkan
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.frag
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.vk.frag --type f --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows --profile spirv
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.vert
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.vk.vert --type v --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows --profile spirv
  # Direct3D 9
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.frag
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.d3d9.frag --type f --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_3_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.vert
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.d3d9.vert --type v --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_3_0 -O 3
  # Direct3D 11
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.frag
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.d3d11.frag --type f --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.vert
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.d3d11.vert --type v --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3
  # Direct3D 12
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.frag
    -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.d3d12.frag --type f --varyingdef
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f
    ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.field.shadow.vert -o ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.field.shadow.d3d12.vert --type v
    --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3)

# OVERLAY SHADERS
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
  # OpenGL
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --profile 120
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --profile 120
  # Vulkan
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform linux --profile spirv
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform linux --profile spirv
  # Direct3D 9
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.d3d9.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_3_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.d3d9.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_3_0 -O 3
  # Direct3D 11
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_5_0 -O 3
  # Direct3D 12
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.overlay.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_5_0 -O 3)

# POST SHADERS
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
  # OpenGL
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --profile 120
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --profile 120
  # Vulkan
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform linux --profile spirv
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform linux --profile spirv
  # Direct3D 9
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.d3d9.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_3_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.d3d9.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_3_0 -O 3
  # Direct3D 11
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_5_0 -O 3
  # Direct3D 12
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p ps_5_0 -O 3
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/bgfx/shadercRelease -i ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o
    ${CMAKE_BINARY_DIR}/bin/shaders/FFNx.post.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc
    --platform windows -p vs_5_0 -O 3)

# INSTALL
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
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
)
