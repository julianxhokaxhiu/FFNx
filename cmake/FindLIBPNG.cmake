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

if (NOT LIBPNG_FOUND)
	find_package(ZLib REQUIRED)

	find_library(
		LIBPNG_LIBRARY
		libpng16
		PATH_SUFFIXES
		lib
	)

	find_path(
		LIBPNG_INCLUDE_DIR
		png.h
		PATH_SUFFIXES
		include
	)

	add_library(LIBPNG::LIBPNG STATIC IMPORTED)

	set_target_properties(
		LIBPNG::LIBPNG
		PROPERTIES
		IMPORTED_LOCATION
		"${LIBPNG_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${LIBPNG_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"ZLib::ZLib"
	)

	find_package_handle_standard_args(LIBPNG DEFAULT_MSG LIBPNG_LIBRARY LIBPNG_INCLUDE_DIR)
endif()