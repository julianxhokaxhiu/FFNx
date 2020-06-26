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

set(LIBCONFUSE_VERSION "3.2.3")

ExternalProject_Add(
    libconfuse
    URL                 "https://github.com/julianxhokaxhiu/libconfuse/releases/download/v${LIBCONFUSE_VERSION}/confuse-${LIBCONFUSE_VERSION}.0.zip"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	PATCH_COMMAND		"${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third_party/libconfuse/CMakeLists.txt.in" "<SOURCE_DIR>/CMakeLists.txt"
	CMAKE_ARGS
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_PREFIX_PATH=<INSTALL_DIR>"
		"-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
		"-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
		"-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
		"-DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}"
)
