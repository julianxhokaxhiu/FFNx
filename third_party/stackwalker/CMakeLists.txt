include(ExternalProject)

set(STACKWALKER_VERSION "1.2.1")

ExternalProject_Add(
	StackWalker
	URL						"https://github.com/julianxhokaxhiu/StackWalkerCI/releases/download/${STACKWALKER_VERSION}/StackWalker-v${STACKWALKER_VERSION}.0_Debug.zip"
	INSTALL_DIR				"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND		""
	BUILD_COMMAND			""
	INSTALL_COMMAND			"${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>" "<INSTALL_DIR>"
)
