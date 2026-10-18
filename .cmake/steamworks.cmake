find_package(STEAMWORKSSDK CONFIG REQUIRED)

get_filename_component(FFNX_STEAM_API_LIB_DIR "${STEAMWORKSSDK_LIBRARY}" DIRECTORY)
get_filename_component(FFNX_STEAMWORKSSDK_ROOT "${FFNX_STEAM_API_LIB_DIR}" DIRECTORY)
set(FFNX_STEAM_API_DLL "${FFNX_STEAMWORKSSDK_ROOT}/tools/steamworkssdk/steam_api.dll")
if(NOT EXISTS "${FFNX_STEAM_API_DLL}")
  message(FATAL_ERROR "Steamworks SDK runtime DLL not found: ${FFNX_STEAM_API_DLL}")
endif()

get_filename_component(FFNX_MSVC_BIN_DIR "${CMAKE_AR}" DIRECTORY)
find_program(FFNX_DUMPBIN_EXECUTABLE
  NAMES dumpbin.exe
  PATHS "${FFNX_MSVC_BIN_DIR}"
  NO_DEFAULT_PATH
  REQUIRED
)

set(FFNX_STEAM_API_IMPORT_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/FFNx_steam_api.lib")
set(FFNX_STEAM_API_DEF_FILE "${CMAKE_CURRENT_BINARY_DIR}/FFNx_steam_api.def")
add_custom_command(
  OUTPUT "${FFNX_STEAM_API_IMPORT_LIBRARY}"
  BYPRODUCTS "${FFNX_STEAM_API_DEF_FILE}"
  COMMAND "${CMAKE_COMMAND}"
          "-DDUMPBIN=${FFNX_DUMPBIN_EXECUTABLE}"
          "-DSTEAM_API_DLL=${FFNX_STEAM_API_DLL}"
          "-DOUTPUT_DEF=${FFNX_STEAM_API_DEF_FILE}"
          -P "${CMAKE_CURRENT_LIST_DIR}/GenerateSteamImportDef.cmake"
  COMMAND "${CMAKE_AR}" /NOLOGO
          "/DEF:${FFNX_STEAM_API_DEF_FILE}"
          /MACHINE:X86
          "/OUT:${FFNX_STEAM_API_IMPORT_LIBRARY}"
  DEPENDS
          "${CMAKE_CURRENT_LIST_DIR}/GenerateSteamImportDef.cmake"
          "${FFNX_STEAM_API_DLL}"
  VERBATIM
)
add_custom_target(FFNxSteamAPIImportLibrary DEPENDS "${FFNX_STEAM_API_IMPORT_LIBRARY}")
set_target_properties(STEAMWORKSSDK::STEAMWORKSSDK PROPERTIES
  IMPORTED_LOCATION "${FFNX_STEAM_API_IMPORT_LIBRARY}"
)

function(ffnx_setup_steamworks target)
  target_link_libraries(${target} STEAMWORKSSDK::STEAMWORKSSDK delayimp)
  target_link_options(${target} PRIVATE /DELAYLOAD:FFNx_steam_api.dll)
  add_dependencies(${target} FFNxSteamAPIImportLibrary)
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/bin"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${FFNX_STEAM_API_DLL}"
            "${CMAKE_BINARY_DIR}/bin/FFNx_steam_api.dll"
    VERBATIM
  )
endfunction()
