include(ExternalProject)

set(IMGUI_VERSION "1.76")

ExternalProject_Add(
	imgui
	URL                 "https://github.com/julianxhokaxhiu/imguiCI/releases/download/${IMGUI_VERSION}/imgui-v${IMGUI_VERSION}.0.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/lib" "<INSTALL_DIR>/lib"
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/include" "<INSTALL_DIR>/include"
)
