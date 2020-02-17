include(FindPackageHandleStandardArgs)

if (NOT VPX_FOUND)
	find_library(
		VPX_LIBRARY
		vpxmt vpxmtd
		PATH_SUFFIXES
		lib
	)

	find_path(
		VPX_INCLUDE_DIR
		vpx
		PATH_SUFFIXES
		include
	)

	add_library(VPX::VPX STATIC IMPORTED)

	set_target_properties(
		VPX::VPX
		PROPERTIES
		IMPORTED_LOCATION
		"${VPX_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${VPX_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(VPX DEFAULT_MSG VPX_LIBRARY VPX_INCLUDE_DIR)
endif()
