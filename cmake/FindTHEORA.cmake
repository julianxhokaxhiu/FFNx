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

if(NOT THEORA_FOUND)
  find_package(OGG REQUIRED)

  # Common
  find_path(THEORA_INCLUDE_DIR theora PATH_SUFFIXES include)

  # Encoder
  find_library(THEORAENC_LIBRARY theoraenc PATH_SUFFIXES lib)

  add_library(THEORA::THEORAENC STATIC IMPORTED)

  set_target_properties(
    THEORA::THEORAENC
    PROPERTIES IMPORTED_LOCATION "${THEORAENC_LIBRARY}"
               INTERFACE_INCLUDE_DIRECTORIES "${THEORA_INCLUDE_DIR}"
               INTERFACE_LINK_LIBRARIES "OGG::OGG")

  # Decoder
  find_library(THEORADEC_LIBRARY theoradec PATH_SUFFIXES lib)

  add_library(THEORA::THEORADEC STATIC IMPORTED)

  set_target_properties(THEORA::THEORADEC PROPERTIES IMPORTED_LOCATION "${THEORADEC_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${THEORA_INCLUDE_DIR}")

  find_package_handle_standard_args(THEORA DEFAULT_MSG THEORAENC_LIBRARY THEORADEC_LIBRARY THEORA_INCLUDE_DIR)
endif()
