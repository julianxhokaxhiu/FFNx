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

include(FindPackageHandleStandardArgs)

if (NOT X264_FOUND)
	find_library(
		X264_LIBRARY
		libx264
		PATH_SUFFIXES
		lib
	)

	find_path(
		X264_INCLUDE_DIR
		z264.h x264_config.h
		PATH_SUFFIXES
		include
	)

	add_library(X264::X264 STATIC IMPORTED)

	set_target_properties(
		X264::X264
		PROPERTIES
		IMPORTED_LOCATION
		"${X264_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${X264_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"bcrypt"
	)

	find_package_handle_standard_args(X264 DEFAULT_MSG X264_LIBRARY X264_INCLUDE_DIR)
endif()
