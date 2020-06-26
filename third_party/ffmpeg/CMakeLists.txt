#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 Julian Xhokaxhiu                                      #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
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

include(ExternalProject)

set(FFMPEG_VERSION "4.3.0")

ExternalProject_Add(
	ffmpeg
	URL                 "https://github.com/julianxhokaxhiu/ffmpegCI/releases/download/${FFMPEG_VERSION}/ffmpeg-v${FFMPEG_VERSION}.0.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	CONFIGURE_COMMAND	""
	BUILD_COMMAND		""
	INSTALL_COMMAND
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/lib" "<INSTALL_DIR>/lib"
		COMMAND "${CMAKE_COMMAND}" -E copy_directory "<SOURCE_DIR>/installed/x86-windows-static/include" "<INSTALL_DIR>/include"
)
