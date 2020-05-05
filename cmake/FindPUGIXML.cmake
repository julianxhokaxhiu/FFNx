include(FindPackageHandleStandardArgs)

if (NOT PUGIXML_FOUND)
	find_path(
		PUGIXML_INCLUDE_DIR
		pugixml.hpp
		PATH_SUFFIXES
		include
	)

	find_library(
		PUGIXML_LIBRARY
		pugixml
		PATH_SUFFIXES
		lib
	)

	add_library(PUGIXML::PUGIXML STATIC IMPORTED)

	set_target_properties(
		PUGIXML::PUGIXML
		PROPERTIES
		IMPORTED_LOCATION
		"${PUGIXML_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${PUGIXML_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(PUGIXML DEFAULT_MSG
		# PUGIXML
		PUGIXML_LIBRARY
		PUGIXML_INCLUDE_DIR
  )
endif()
