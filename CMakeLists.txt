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

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE) # Azure and multi-configuration generators don't set CMAKE_BUILD_TYPE
	if (NOT CMAKE_BUILD_TYPE MATCHES Release AND NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
		message(FATAL_ERROR "You selected `${CMAKE_BUILD_TYPE}` mode. Only `Release` and `RelWithDebInfo` are supported.\nAdd -DCMAKE_BUILD_TYPE=Release or -DCMAKE_BUILD_TYPE=RelWithDebInfo to your cmake command line.")
	endif()
endif()
if (NOT DEFINED _DLL_VERSION OR NOT _DLL_VERSION)
	message(FATAL_ERROR "_DLL_VERSION must be set to continue building with cmake. \nExample: Add -D_DLL_VERSION=devel to your cmake command line.")
endif ()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /NODEFAULTLIB:LIBCMTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4088,4099"
)
set(_DLL_VERSION "${_DLL_VERSION}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(FORCEHEAP "Force all allocation to our heap" OFF)
if(FORCEHEAP)
  add_definitions(-DNO_EXT_HEAP)
endif()

option(TRACEHEAP "Trace and keep count of every allocation made by this program" OFF)
if(TRACEHEAP)
  add_definitions(-DHEAP_DEBUG)
endif()

option(PROFILING "Enable Profiling" OFF)
if(PROFILING)
  add_definitions(-DPROFILE)
endif()

option(SUPERBUILD "Build the project using a superbuild" ON)

if(SUPERBUILD)
  project(SUPERBUILD)
  set_directory_properties(PROPERTIES EP_BASE "${CMAKE_BINARY_DIR}/ep")
  add_subdirectory(third_party)
  include(ExternalProject)
  ExternalProject_Add(
    FFNx
    SOURCE_DIR "${CMAKE_SOURCE_DIR}"
    CMAKE_ARGS "-DSUPERBUILD=OFF" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" "-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/vendor"
               "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}" "-D_DLL_VERSION=${_DLL_VERSION}"
    DEPENDS bgfx
            ffmpeg
            tomlplusplus
            vgmstream
            StackWalker
            pugixml
            libpng
            imgui
            soloud
            openpsf)
  ExternalProject_Add_Step(
    FFNx reconfigure
    COMMAND ${CMAKE_COMMAND} -E echo "Forcing a superbuild reconfigure"
    DEPENDEES download
    DEPENDERS configure
    ALWAYS ON)
  return()
endif()

project(FFNx)
add_subdirectory(src)
