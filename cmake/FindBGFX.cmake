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

if(NOT BGFX_FOUND)
  find_library(BGFX_LIBRARY bgfxRelease PATH_SUFFIXES lib)

  find_path(BGFX_INCLUDE_DIR bgfx PATH_SUFFIXES include)

  list(APPEND BGFX_INCLUDE_DIRS ${BGFX_INCLUDE_DIR})
  list(APPEND BGFX_INCLUDE_DIRS ${BGFX_INCLUDE_DIR}/compat/msvc)

  add_library(BGFX::BGFX STATIC IMPORTED)

  set_target_properties(BGFX::BGFX PROPERTIES IMPORTED_LOCATION "${BGFX_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${BGFX_INCLUDE_DIRS}")

  #-----------------------------------------

  find_library(BX_LIBRARY bxRelease PATH_SUFFIXES lib)

  find_path(BX_INCLUDE_DIR bx PATH_SUFFIXES include)

  add_library(BGFX::BX STATIC IMPORTED)

  set_target_properties(BGFX::BX PROPERTIES IMPORTED_LOCATION "${BX_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${BX_INCLUDE_DIR}")

  #-----------------------------------------

  find_library(BIMG_LIBRARY bimgRelease PATH_SUFFIXES lib)

  find_path(BIMG_INCLUDE_DIR bimg PATH_SUFFIXES include)

  add_library(BGFX::BIMG STATIC IMPORTED)

  set_target_properties(BGFX::BIMG PROPERTIES IMPORTED_LOCATION "${BIMG_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${BIMG_INCLUDE_DIR}")

  #-----------------------------------------

  find_library(BIMG_DECODE_LIBRARY bimg_decodeRelease PATH_SUFFIXES lib)

  add_library(BGFX::BIMG_DECODE STATIC IMPORTED)

  set_target_properties(BGFX::BIMG_DECODE PROPERTIES IMPORTED_LOCATION "${BIMG_DECODE_LIBRARY}")

  #-----------------------------------------

  find_library(BIMG_ENCODE_LIBRARY bimg_encodeRelease PATH_SUFFIXES lib)

  add_library(BGFX::BIMG_ENCODE STATIC IMPORTED)

  set_target_properties(BGFX::BIMG_ENCODE PROPERTIES IMPORTED_LOCATION "${BIMG_ENCODE_LIBRARY}")

  #-----------------------------------------

  find_package_handle_standard_args(
    BGFX
    DEFAULT_MSG
    BGFX_LIBRARY
    BGFX_INCLUDE_DIR
    BX_LIBRARY
    BX_INCLUDE_DIR
    BIMG_LIBRARY
    BIMG_INCLUDE_DIR
    BIMG_DECODE_LIBRARY
    BIMG_ENCODE_LIBRARY)
endif()
