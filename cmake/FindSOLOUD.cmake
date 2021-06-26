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

if(NOT SOLOUD_FOUND)
  find_library(SOLOUD_LIBRARY soloud_static_x86 PATH_SUFFIXES lib/soloud)

  find_path(SOLOUD_INCLUDE_DIR soloud PATH_SUFFIXES include)

  add_library(SOLOUD::SOLOUD STATIC IMPORTED)

  set_target_properties(SOLOUD::SOLOUD PROPERTIES IMPORTED_LOCATION "${SOLOUD_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${SOLOUD_INCLUDE_DIRS}")

  find_package_handle_standard_args(SOLOUD DEFAULT_MSG SOLOUD_LIBRARY SOLOUD_INCLUDE_DIR)
endif()
