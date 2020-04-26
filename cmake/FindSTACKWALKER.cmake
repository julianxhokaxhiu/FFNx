include(FindPackageHandleStandardArgs)

if (NOT STACKWALKER_FOUND)
	find_path(
		STACKWALKER_INCLUDE_DIR
		StackWalker.h
		PATH_SUFFIXES
		include
	)

	find_library(
		STACKWALKER_LIBRARY
		StackWalker
		PATH_SUFFIXES
		lib
	)

	add_library(STACKWALKER::STACKWALKER STATIC IMPORTED)

	set_target_properties(
		STACKWALKER::STACKWALKER
		PROPERTIES
		IMPORTED_LOCATION
		"${STACKWALKER_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${STACKWALKER_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(STACKWALKER DEFAULT_MSG
		# STACKWALKER
		STACKWALKER_LIBRARY
		STACKWALKER_INCLUDE_DIR
  )
endif()
