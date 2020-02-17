include(FindPackageHandleStandardArgs)

if (NOT PNG_FOUND)
	find_package(ZLib REQUIRED)

	find_library(
		PNG_LIBRARY
		libpng_static libpng16_static libpng16_staticd libpng libpng16 png png16
		PATH_SUFFIXES
		lib
	)

	find_path(
		PNG_INCLUDE_DIR
		png.h
		PATH_SUFFIXES
		include
	)

	add_library(PNG::PNG STATIC IMPORTED)

	set_target_properties(
		PNG::PNG
		PROPERTIES
		IMPORTED_LOCATION
		"${PNG_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${PNG_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"ZLib::ZLib"
	)

	find_package_handle_standard_args(PNG DEFAULT_MSG PNG_LIBRARY PNG_INCLUDE_DIR)
endif()