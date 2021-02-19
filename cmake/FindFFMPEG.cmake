#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
#    Copyright (C) 2021 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of FFNx                                                #
#                                                                             #
#    FFNx is free software: you can redistribute it and/or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    FFNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

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
	find_package(OPUS REQUIRED)
	find_package(SPEEX REQUIRED)
	find_package(THEORA REQUIRED)
	find_package(VORBIS REQUIRED)
	find_package(VPX REQUIRED)
	find_package(WAVPACK REQUIRED)
	find_package(X264 REQUIRED)
	find_package(X265 REQUIRED)

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
		"FFMPEG::AVUtil;FFMPEG::SWResample;X264::X264;X265::X265;VPX::VPX;OPUS::OPUS;SPEEX::SPEEX;THEORA::THEORAENC;THEORA::THEORADEC;VORBIS::VORBIS;WAVPACK::WAVPACK;mfplat;mfuuid"
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
