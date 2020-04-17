include(ExternalProject)

set(FFMPEG_VERSION "4.2.2")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(FFMPEG_LIB_DIR "<SOURCE_DIR>/installed/x86-msvc-v142-static/lib")
else()
	set(FFMPEG_LIB_DIR "<SOURCE_DIR>/installed/x86-msvc-v142-static/debug/lib")
endif()

ExternalProject_Add(
	ffmpeg
	URL                 "https://github.com/julianxhokaxhiu/ffmpegCI/releases/download/${FFMPEG_VERSION}/ffmpeg-v${FFMPEG_VERSION}.0.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "${FFMPEG_LIB_DIR}" "<INSTALL_DIR>/lib"
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-msvc-v142-static/include" "<INSTALL_DIR>/include"
)
