include(ExternalProject)

set(BGFX_VERSION "1.0.0")

ExternalProject_Add(
	bgfx
	URL                 "https://github.com/julianxhokaxhiu/bgfxCI/releases/download/${BGFX_VERSION}/bgfx-v${BGFX_VERSION}_Release.zip"
	INSTALL_DIR		    "${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND		"${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>" "<INSTALL_DIR>"
)
