include(ExternalProject)

set(BGFX_VERSION "1.3.0")

set(BGFX_RELEASE "Debug")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BGFX_RELEASE "Release")
endif()

ExternalProject_Add(
	bgfx
	URL                 "https://github.com/julianxhokaxhiu/bgfxCI/releases/download/${BGFX_VERSION}/bgfx-v${BGFX_VERSION}_${BGFX_RELEASE}.zip"
	INSTALL_DIR		    "${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND		"${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>" "<INSTALL_DIR>"
)
