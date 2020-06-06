include(ExternalProject)

set(FFMPEG_VERSION "4.2.3")

ExternalProject_Add(
	ffmpeg
	URL                 "https://github.com/julianxhokaxhiu/ffmpegCI/releases/download/${FFMPEG_VERSION}/ffmpeg-v${FFMPEG_VERSION}.0.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/debug/lib" "<INSTALL_DIR>/lib"
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/include" "<INSTALL_DIR>/include"
)
