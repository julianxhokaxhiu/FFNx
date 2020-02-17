include(FindPackageHandleStandardArgs)

if (NOT ZLIB_FOUND)
	find_library(
		ZLIB_LIBRARY
		zlibd zlib
		PATH_SUFFIXES
		lib
	)

	find_path(
		ZLIB_INCLUDE_DIR
		zlib.h
		PATH_SUFFIXES
		include
	)

	add_library(ZLib::ZLib STATIC IMPORTED)

	set_target_properties(
		ZLib::ZLib
		PROPERTIES
		IMPORTED_LOCATION
		"${ZLIB_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${ZLIB_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(ZLib DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
endif()
