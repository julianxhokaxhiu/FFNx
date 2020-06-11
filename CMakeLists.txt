cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /NODEFAULTLIB:LIBCMTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4088,4099")
set(_DLL_VERSION "${_DLL_VERSION}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(FORCEHEAP "Force all allocation to our heap" OFF)
if (FORCEHEAP)
    add_definitions(-DNO_EXT_HEAP)
endif()

option(TRACEHEAP "Trace and keep count of every allocation made by this program" OFF)
if (TRACEHEAP)
    add_definitions(-DHEAP_DEBUG)
endif()

option(PROFILING "Enable Profiling" OFF)
if (PROFILING)
    add_definitions(-DPROFILE)
endif()

option(SUPERBUILD "Build the project using a superbuild" ON)

if (SUPERBUILD)
	project(SUPERBUILD)
	set_directory_properties(PROPERTIES EP_BASE "${CMAKE_BINARY_DIR}/ep")
	add_subdirectory(third_party)
	include(ExternalProject)
	ExternalProject_Add(
		FFNx
		SOURCE_DIR	"${CMAKE_SOURCE_DIR}"
		CMAKE_ARGS
			"-DSUPERBUILD=OFF"
			"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/vendor"
			"-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
			"-D_DLL_VERSION=${_DLL_VERSION}"
		DEPENDS
		bgfx
		ffmpeg
		libconfuse
		vgmstream
		StackWalker
		pugixml
		libpng
	)
	ExternalProject_Add_Step(
		FFNx
		reconfigure
		COMMAND ${CMAKE_COMMAND} -E echo "Forcing a superbuild reconfigure"
		DEPENDEES download
		DEPENDERS configure
		ALWAYS ON
	)
	return()
endif()

project(FFNx)
add_subdirectory(src)

