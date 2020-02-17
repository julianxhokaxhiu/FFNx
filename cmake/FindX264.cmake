include(FindPackageHandleStandardArgs)

if (NOT X264_FOUND)
	find_library(
		X264_LIBRARY
		libx264
		PATH_SUFFIXES
		lib
	)

	find_path(
		X264_INCLUDE_DIR
		z264.h x264_config.h
		PATH_SUFFIXES
		include
	)

	add_library(X264::X264 STATIC IMPORTED)

	set_target_properties(
		X264::X264
		PROPERTIES
		IMPORTED_LOCATION
		"${X264_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${X264_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"bcrypt"
	)

	find_package_handle_standard_args(X264 DEFAULT_MSG X264_LIBRARY X264_INCLUDE_DIR)
endif()
