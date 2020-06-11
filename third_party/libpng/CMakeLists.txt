include(ExternalProject)

set(LIBPNG_VERSION "1.6.37")

ExternalProject_Add(
	libpng
	URL                 "https://github.com/julianxhokaxhiu/libpngCI/releases/download/${LIBPNG_VERSION}/libpng-v${LIBPNG_VERSION}.0.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/lib" "<INSTALL_DIR>/lib"
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/include" "<INSTALL_DIR>/include"
)
