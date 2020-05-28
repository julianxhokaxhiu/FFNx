include(ExternalProject)

set(PUGIXML_VERSION "1.10")

ExternalProject_Add(
	pugixml
	URL						"https://github.com/julianxhokaxhiu/pugixmlCI/releases/download/${PUGIXML_VERSION}/pugixml-v${PUGIXML_VERSION}.0_Debug.zip"
	INSTALL_DIR				"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND		""
	BUILD_COMMAND			""
	INSTALL_COMMAND			"${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>" "<INSTALL_DIR>"
)
