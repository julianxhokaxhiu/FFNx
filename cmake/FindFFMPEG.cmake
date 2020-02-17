include(FindPackageHandleStandardArgs)

if (NOT FFMPEG_FOUND)
	# avutil
	find_library(
		FFMPEG_AVUTIL_LIBRARY
		avutil
		PATH_SUFFIXES
		lib
	)

	find_path(
		FFMPEG_AVUTIL_INCLUDE_DIR
		libavutil/avutil.h
		PATH_SUFFIXES
		include
	)

	add_library(FFMPEG::AVUtil STATIC IMPORTED)

	set_target_properties(
		FFMPEG::AVUtil
		PROPERTIES
		IMPORTED_LOCATION
		"${FFMPEG_AVUTIL_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${FFMPEG_AVUTIL_INCLUDE_DIR}"
	)

	# swresample
	find_library(
		FFMPEG_SWRESAMPLE_LIBRARY
		swresample
		PATH_SUFFIXES
		lib
	)

	find_path(
		FFMPEG_SWRESAMPLE_INCLUDE_DIR
		libswresample/swresample.h
		PATH_SUFFIXES
		include
	)

	add_library(FFMPEG::SWResample STATIC IMPORTED)

	set_target_properties(
		FFMPEG::SWResample
		PROPERTIES
		IMPORTED_LOCATION
		"${FFMPEG_SWRESAMPLE_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${FFMPEG_SWRESAMPLE_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		FFMPEG::AVUtil
	)

	# avcodec
	find_package(X264 REQUIRED)
	find_package(VPX REQUIRED)

	find_library(
		FFMPEG_AVCODEC_LIBRARY
		avcodec
		PATH_SUFFIXES
		lib
	)

	find_path(
		FFMPEG_AVCODEC_INCLUDE_DIR
		libavcodec/avcodec.h
		PATH_SUFFIXES
		include
	)

	add_library(FFMPEG::AVCodec STATIC IMPORTED)

	set_target_properties(
		FFMPEG::AVCodec
		PROPERTIES
		IMPORTED_LOCATION
		"${FFMPEG_AVCODEC_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${FFMPEG_AVCODEC_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"FFMPEG::AVUtil;FFMPEG::SWResample;X264::X264;VPX::VPX"
	)

	# avformat
	find_library(
		FFMPEG_AVFORMAT_LIBRARY
		avformat
		PATH_SUFFIXES
		lib
	)

	find_path(
		FFMPEG_AVFORMAT_INCLUDE_DIR
		libavformat/avformat.h
		PATH_SUFFIXES
		include
	)

	add_library(FFMPEG::AVFormat STATIC IMPORTED)

	set_target_properties(
		FFMPEG::AVFormat
		PROPERTIES
		IMPORTED_LOCATION
		"${FFMPEG_AVFORMAT_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${FFMPEG_AVFORMAT_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"FFMPEG::AVUtil;FFMPEG::AVCodec;wsock32;ws2_32;secur32"
	)

	# swscale
	find_library(
		FFMPEG_SWSCALE_LIBRARY
		swscale
		PATH_SUFFIXES
		lib
	)

	find_path(
		FFMPEG_SWSCALE_INCLUDE_DIR
		libswscale/swscale.h
		PATH_SUFFIXES
		include
	)

	add_library(FFMPEG::SWScale STATIC IMPORTED)

	set_target_properties(
		FFMPEG::SWScale
		PROPERTIES
		IMPORTED_LOCATION
		"${FFMPEG_SWSCALE_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${FFMPEG_SWSCALE_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		FFMPEG::AVUtil
	)

	find_package_handle_standard_args(
		FFMPEG
		DEFAULT_MSG
		FFMPEG_AVCODEC_LIBRARY
		FFMPEG_AVFORMAT_LIBRARY
		FFMPEG_AVUTIL_LIBRARY
		FFMPEG_SWSCALE_LIBRARY
		FFMPEG_SWRESAMPLE_LIBRARY
		)
endif()
