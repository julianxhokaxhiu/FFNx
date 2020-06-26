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

if(NOT CONFUSE_FOUND)
	find_library(
		CONFUSE_LIBRARY
		libconfuse
		PATH_SUFFIXES
		lib
	)

	find_path(
		CONFUSE_INCLUDE_DIR
		confuse.h
		PATH_SUFFIXES
		include
	)

	add_library(CONFUSE::CONFUSE STATIC IMPORTED)

	set_target_properties(
		CONFUSE::CONFUSE
		PROPERTIES
		IMPORTED_LOCATION
		"${CONFUSE_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${CONFUSE_INCLUDE_DIR}"
		INTERFACE_COMPILE_DEFINITIONS
		BUILDING_STATIC=1
	)

	find_package_handle_standard_args(CONFUSE DEFAULT_MSG CONFUSE_LIBRARY CONFUSE_INCLUDE_DIR)
endif()