include(ExternalProject)

set(BGFX_RELEASE "Debug")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BGFX_RELEASE "Release")
endif()

ExternalProject_Add(
	bgfx
	URL                 "https://github.com/julianxhokaxhiu/bgfxCI/releases/download/1.0.0/bgfx-v1.0.0_${BGFX_RELEASE}.zip"
	INSTALL_DIR		    "${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND		"${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>" "<INSTALL_DIR>"
)
