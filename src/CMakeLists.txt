find_package(BGFX REQUIRED)
find_package(CONFUSE REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(VGMSTREAM REQUIRED)
find_package(STACKWALKER REQUIRED)
find_package(PUGIXML REQUIRED)

set(RELEASE_NAME "FFNx")

# Include all the source code files
file(GLOB_RECURSE source_files "*.cpp")

add_library(${RELEASE_NAME} SHARED ${source_files})
target_include_directories(${RELEASE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${RELEASE_NAME}
	dbghelp
	shlwapi
    psapi
    winmm
    shcore
	BGFX::BGFX
    BGFX::BIMG
    BGFX::BIMG_DECODE
    BGFX::BIMG_ENCODE
    BGFX::BX
	CONFUSE::CONFUSE
	FFMPEG::AVUtil
	FFMPEG::SWResample
	FFMPEG::AVCodec
	FFMPEG::AVFormat
	FFMPEG::SWScale
	VGMSTREAM::VGMSTREAM
    STACKWALKER::STACKWALKER
    XInput
    PUGIXML::PUGIXML
)
target_compile_options(${RELEASE_NAME}
	PRIVATE /D_CRT_SECURE_NO_WARNINGS
	PRIVATE /DNOMINMAX
	PRIVATE /EHsc
)
target_link_options(${RELEASE_NAME}
    PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb
    PRIVATE /DEF:${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.def
)
if(_DLL_VERSION)
	target_compile_options(${RELEASE_NAME} PRIVATE /DVERSION="${_DLL_VERSION}")
endif()

# SHADER COMPILATION
add_custom_command(
    TARGET ${RELEASE_NAME}
    POST_BUILD
    # ensure bin directory exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin/shaders
    # OpenGL
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.gl.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.gl.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc
	# Vulkan
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform linux --profile spirv
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform linux --profile spirv
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.vk.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform linux --profile spirv
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.vk.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform linux --profile spirv
    # Direct3D 11
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p vs_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d11.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d11.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p vs_5_0 -O 3
    # Direct3D 12
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p vs_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.frag -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d12.frag --type f --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p ps_5_0 -O 3
    COMMAND ${CMAKE_INSTALL_PREFIX}/vendor/bin/shaderc -i ${CMAKE_INSTALL_PREFIX}/vendor/include -f ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.post.vert -o ${CMAKE_INSTALL_PREFIX}/bin/shaders/FFNx.post.d3d12.vert --type v --varyingdef ${CMAKE_SOURCE_DIR}/misc/${RELEASE_NAME}.varying.def.sc --platform windows -p vs_5_0 -O 3
)

# INSTALL
install(TARGETS ${RELEASE_NAME} RUNTIME DESTINATION bin)
install(FILES ../COPYING.TXT DESTINATION bin OPTIONAL)
install(FILES $<TARGET_PDB_FILE:${RELEASE_NAME}> DESTINATION bin OPTIONAL)
install(DIRECTORY ../misc/hext DESTINATION bin OPTIONAL)
install(FILES ../misc/${RELEASE_NAME}.cfg DESTINATION bin OPTIONAL)
install(FILES ../misc/FF7.reg DESTINATION bin OPTIONAL)
install(FILES ../misc/FF8.reg DESTINATION bin OPTIONAL)
install(FILES ../misc/eax.dll DESTINATION bin OPTIONAL)
