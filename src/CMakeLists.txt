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
#    FFNx is free software: you can redistribute it and/or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    FFNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

find_package(BGFX REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(VGMSTREAM REQUIRED)
find_package(STACKWALKER REQUIRED)
find_package(PUGIXML REQUIRED)
find_package(LIBPNG REQUIRED)
find_package(IMGUI REQUIRED)
find_package(SOLOUD REQUIRED)
find_package(OPENPSF REQUIRED)

set(RELEASE_NAME "FFNx")

if (_DLL_VERSION STREQUAL "devel" OR _DLL_VERSION MATCHES "-" )
    set(_DLL_RCVERSION "0,0,0,0")
    set(_DLL_RCSTRVERSION "0.0.0.0")
else()
    string(REPLACE "." "," _DLL_RCVERSION ${_DLL_VERSION})
    set(_DLL_RCSTRVERSION ${_DLL_VERSION})
endif()

# Include all the source code files
file(GLOB_RECURSE source_files "*.cpp")

configure_file(
  ../misc/version.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
  @ONLY
)

add_library(${RELEASE_NAME} SHARED ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
target_include_directories(${RELEASE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${RELEASE_NAME}
	dbghelp
	shlwapi
    psapi
    winmm
	BGFX::BGFX
    BGFX::BIMG
    BGFX::BIMG_DECODE
    BGFX::BIMG_ENCODE
    BGFX::BX
	FFMPEG::AVUtil
	FFMPEG::SWResample
	FFMPEG::AVCodec
	FFMPEG::AVFormat
	FFMPEG::SWScale
	VGMSTREAM::VGMSTREAM
    STACKWALKER::STACKWALKER
    XINPUT9_1_0
    PUGIXML::PUGIXML
    LIBPNG::LIBPNG
    IMGUI::IMGUI
    SOLOUD::SOLOUD
    OPENPSF::PSXCORE
    OPENPSF::PSFLIB
    OPENPSF::OPENPSF
)
target_compile_options(${RELEASE_NAME}
    PRIVATE /DDIRECTINPUT_VERSION=0x0800
    PRIVATE /DVERSION="${_DLL_VERSION}"
	PRIVATE /D_CRT_SECURE_NO_WARNINGS
	PRIVATE /DNOMINMAX
	PRIVATE /EHsc
    PRIVATE /std:c++latest
    PRIVATE /w
    PRIVATE /Ox
)
target_link_options(${RELEASE_NAME}
    PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb
    PRIVATE /DEF:${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.def
)

# SHADER COMPILATION

# GAME SHADERS
foreach(BGFX_VARYING flat smooth)
    add_custom_command(
        TARGET ${RELEASE_NAME}
        POST_BUILD
        # ensure bin directory exists
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin/shaders
        # OpenGL
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --profile 120
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --profile 120
	    # Vulkan
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows --profile spirv
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows --profile spirv
        # Direct3D 9
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.d3d9.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_3_0 -O 3
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.d3d9.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_3_0 -O 3
        # Direct3D 11
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_5_0 -O 3
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_5_0 -O 3
        # Direct3D 12
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p ps_5_0 -O 3
        COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.${BGFX_VARYING}.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.${BGFX_VARYING}.def.sc --platform windows -p vs_5_0 -O 3
    )
endforeach()

# OVERLAY SHADERS
add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    # ensure bin directory exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin/shaders
    # OpenGL
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --profile 120
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --profile 120
	# Vulkan
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform linux --profile spirv
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform linux --profile spirv
    # Direct3D 9
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.d3d9.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_3_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.d3d9.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_3_0 -O 3
    # Direct3D 11
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3
    # Direct3D 12
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.overlay.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.overlay.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3
)

# POST SHADERS
add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    # ensure bin directory exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin/shaders
    # OpenGL
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --profile 120
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --profile 120
	# Vulkan
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform linux --profile spirv
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform linux --profile spirv
    # Direct3D 9
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d9.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_3_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d9.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_3_0 -O 3
    # Direct3D 11
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3
    # Direct3D 12
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.smooth.def.sc --platform windows -p vs_5_0 -O 3
)

# INSTALL
install(TARGETS ${RELEASE_NAME} RUNTIME DESTINATION bin)
install(FILES ../COPYING.TXT DESTINATION bin OPTIONAL)
install(FILES $<TARGET_PDB_FILE:${RELEASE_NAME}> DESTINATION bin OPTIONAL)
install(DIRECTORY ../misc/hext DESTINATION bin OPTIONAL)
install(FILES ../misc/${RELEASE_NAME}.toml DESTINATION bin OPTIONAL)
install(FILES ../misc/FF7.reg DESTINATION bin OPTIONAL)
install(FILES ../misc/FF8.reg DESTINATION bin OPTIONAL)
# Additional cfg files
install(DIRECTORY DESTINATION bin/sfx OPTIONAL)
install(FILES ../misc/${RELEASE_NAME}.SFX.toml DESTINATION bin/sfx RENAME config.toml OPTIONAL)
install(FILES ../misc/${RELEASE_NAME}.music.toml DESTINATION bin/music/vgmstream RENAME config.toml OPTIONAL)
