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

if (NOT PUGIXML_FOUND)
	find_path(
		PUGIXML_INCLUDE_DIR
		pugixml.hpp
		PATH_SUFFIXES
		include
	)

	find_library(
		PUGIXML_LIBRARY
		pugixml
		PATH_SUFFIXES
		lib
	)

	add_library(PUGIXML::PUGIXML STATIC IMPORTED)

	set_target_properties(
		PUGIXML::PUGIXML
		PROPERTIES
		IMPORTED_LOCATION
		"${PUGIXML_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${PUGIXML_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(PUGIXML DEFAULT_MSG
		# PUGIXML
		PUGIXML_LIBRARY
		PUGIXML_INCLUDE_DIR
  )
endif()
