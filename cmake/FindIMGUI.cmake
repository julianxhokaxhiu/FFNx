include(FindPackageHandleStandardArgs)

if (NOT IMGUI_FOUND)
	find_library(
		IMGUI_LIBRARY
		imgui
		PATH_SUFFIXES
		lib
	)

	find_path(
		IMGUI_INCLUDE_DIR
		imgui.h
		PATH_SUFFIXES
		include
	)

	add_library(IMGUI::IMGUI STATIC IMPORTED)

	set_target_properties(
		IMGUI::IMGUI
		PROPERTIES
		IMPORTED_LOCATION
		"${IMGUI_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${IMGUI_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(IMGUI DEFAULT_MSG IMGUI_LIBRARY IMGUI_INCLUDE_DIR)
endif()