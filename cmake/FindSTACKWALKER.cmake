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

if (NOT STACKWALKER_FOUND)
	find_path(
		STACKWALKER_INCLUDE_DIR
		StackWalker.h
		PATH_SUFFIXES
		include
	)

	find_library(
		STACKWALKER_LIBRARY
		StackWalker
		PATH_SUFFIXES
		lib
	)

	add_library(STACKWALKER::STACKWALKER STATIC IMPORTED)

	set_target_properties(
		STACKWALKER::STACKWALKER
		PROPERTIES
		IMPORTED_LOCATION
		"${STACKWALKER_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${STACKWALKER_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(STACKWALKER DEFAULT_MSG
		# STACKWALKER
		STACKWALKER_LIBRARY
		STACKWALKER_INCLUDE_DIR
  )
endif()
