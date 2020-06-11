include(ExternalProject)

set(VGMSTREAM_VERSION "1.0.0")

ExternalProject_Add(
	vgmstream
	URL                 "https://github.com/julianxhokaxhiu/vgmstream/releases/download/${VGMSTREAM_VERSION}/vgmstream-v${VGMSTREAM_VERSION}_Release.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND		"${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>" "<INSTALL_DIR>"
	DEPENDS
	ffmpeg
)