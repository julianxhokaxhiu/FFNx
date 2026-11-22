cmake_minimum_required(VERSION 4.2)

foreach(required IN ITEMS FFNX_SOURCE_DIR FFNX_BINARY_DIR FFNX_CLANG_FORMAT_EXECUTABLE FFNX_CMAKEFMT_EXECUTABLE FFNX_CMAKEFMT_VERSION)
  if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
    message(FATAL_ERROR "${required} must be set")
  endif()
endforeach()

file(
  GLOB_RECURSE
  sources
  "${FFNX_SOURCE_DIR}/src/*.c"
  "${FFNX_SOURCE_DIR}/src/*.cpp"
  "${FFNX_SOURCE_DIR}/src/*.h"
  "${FFNX_SOURCE_DIR}/src/*.hpp"
  "${FFNX_SOURCE_DIR}/choco/*.c"
  "${FFNX_SOURCE_DIR}/choco/*.cpp"
  "${FFNX_SOURCE_DIR}/choco/*.h"
  "${FFNX_SOURCE_DIR}/choco/*.hpp")
list(FILTER sources EXCLUDE REGEX "/src/imgui_club/")
if(NOT sources)
  message(FATAL_ERROR "No project C/C++ sources found under ${FFNX_SOURCE_DIR}")
endif()

set(arguments -i)
if(FFNX_FORMAT_CHECK)
  set(arguments --dry-run --Werror)
endif()

list(JOIN sources "\n" source_list)
set(source_list_file "${FFNX_BINARY_DIR}/ffnx-format-sources-${FFNX_FORMAT_CHECK}.txt")
file(WRITE "${source_list_file}" "${source_list}\n")
execute_process(
  COMMAND "${FFNX_CLANG_FORMAT_EXECUTABLE}" "--style=file:${FFNX_SOURCE_DIR}/.clang-format" ${arguments} "--files=${source_list_file}"
  COMMAND_ERROR_IS_FATAL ANY)

file(GLOB_RECURSE cmake_sources "${FFNX_SOURCE_DIR}/.cmake/*.cmake")
list(PREPEND cmake_sources "${FFNX_SOURCE_DIR}/CMakeLists.txt")
set(cmake_arguments --in-place)
if(FFNX_FORMAT_CHECK)
  set(cmake_arguments --check)
endif()
execute_process(
  COMMAND
    "${FFNX_CMAKEFMT_EXECUTABLE}"
    --required-version
    "${FFNX_CMAKEFMT_VERSION}"
    --config-file
    "${FFNX_SOURCE_DIR}/.cmakefmt.yaml"
    --verify
    ${cmake_arguments}
    ${cmake_sources}
  COMMAND_ERROR_IS_FATAL ANY)
