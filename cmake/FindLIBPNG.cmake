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