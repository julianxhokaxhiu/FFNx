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

if (NOT VPX_FOUND)
	find_library(
		VPX_LIBRARY
		vpxmt vpxmtd
		PATH_SUFFIXES
		lib
	)

	find_path(
		VPX_INCLUDE_DIR
		vpx
		PATH_SUFFIXES
		include
	)

	add_library(VPX::VPX STATIC IMPORTED)

	set_target_properties(
		VPX::VPX
		PROPERTIES
		IMPORTED_LOCATION
		"${VPX_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${VPX_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(VPX DEFAULT_MSG VPX_LIBRARY VPX_INCLUDE_DIR)
endif()
