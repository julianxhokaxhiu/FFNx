if(NOT DEFINED DUMPBIN OR NOT DEFINED STEAM_API_DLL OR NOT DEFINED OUTPUT_DEF)
  message(FATAL_ERROR "DUMPBIN, STEAM_API_DLL, and OUTPUT_DEF must be specified")
endif()
if(NOT EXISTS "${STEAM_API_DLL}")
  message(FATAL_ERROR "Steam API DLL not found: ${STEAM_API_DLL}")
endif()

execute_process(
  COMMAND "${DUMPBIN}" /NOLOGO /EXPORTS "${STEAM_API_DLL}"
  RESULT_VARIABLE dumpbin_result
  OUTPUT_VARIABLE dumpbin_output
  ERROR_VARIABLE dumpbin_error
)
if(NOT "${dumpbin_result}" STREQUAL "0")
  message(FATAL_ERROR "Failed to read Steam API DLL exports: ${dumpbin_error}")
endif()

string(REPLACE "\r\n" "\n" dumpbin_output "${dumpbin_output}")
string(REGEX MATCHALL "[ \t]*[0-9]+[ \t]+[0-9A-Fa-f]+[ \t]+[0-9A-Fa-f]+[ \t]+[^ \t\n]+" export_lines "${dumpbin_output}")
if(NOT export_lines)
  message(FATAL_ERROR "No named exports found in Steam API DLL: ${STEAM_API_DLL}")
endif()

set(definition "LIBRARY \"FFNx_steam_api.dll\"\nEXPORTS\n")
foreach(export_line IN LISTS export_lines)
  string(REGEX REPLACE "^[ \t]*[0-9]+[ \t]+[0-9A-Fa-f]+[ \t]+[0-9A-Fa-f]+[ \t]+" "" export_name "${export_line}")
  string(APPEND definition "  ${export_name}\n")
endforeach()

file(WRITE "${OUTPUT_DEF}" "${definition}")
