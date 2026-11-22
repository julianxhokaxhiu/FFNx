set(_ffnx_vs_roots "${CMAKE_GENERATOR_INSTANCE}" "$ENV{VSINSTALLDIR}")
if(CMAKE_CXX_COMPILER MATCHES "^(.*)/VC/Tools/MSVC/")
  list(APPEND _ffnx_vs_roots "${CMAKE_MATCH_1}")
endif()

set(_ffnx_clang_format_hints)
foreach(_ffnx_vs_root IN LISTS _ffnx_vs_roots)
  if(_ffnx_vs_root)
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(ARM64|aarch64)$")
      list(APPEND _ffnx_clang_format_hints "${_ffnx_vs_root}/VC/Tools/Llvm/ARM64/bin")
    endif()
    list(APPEND _ffnx_clang_format_hints "${_ffnx_vs_root}/VC/Tools/Llvm/x64/bin" "${_ffnx_vs_root}/VC/Tools/Llvm/bin")
  endif()
endforeach()

find_program(FFNX_CLANG_FORMAT_EXECUTABLE NAMES clang-format HINTS ${_ffnx_clang_format_hints})
if(NOT FFNX_CLANG_FORMAT_EXECUTABLE)
  message(
    FATAL_ERROR
      "clang-format was not found. Install the Visual Studio components from .vsconfig, "
      "or set FFNX_CLANG_FORMAT_EXECUTABLE to Visual Studio's bundled clang-format.exe.")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cmakefmt.cmake")

foreach(_ffnx_format_target IN ITEMS fmt fmt-check)
  set(_ffnx_format_check OFF)
  if(_ffnx_format_target STREQUAL "fmt-check")
    set(_ffnx_format_check ON)
  endif()
  add_custom_target(
    ${_ffnx_format_target}
    COMMAND
      "${CMAKE_COMMAND}"
      "-DFFNX_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
      "-DFFNX_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
      "-DFFNX_CLANG_FORMAT_EXECUTABLE=${FFNX_CLANG_FORMAT_EXECUTABLE}"
      "-DFFNX_CMAKEFMT_EXECUTABLE=${FFNX_CMAKEFMT_EXECUTABLE}"
      "-DFFNX_CMAKEFMT_VERSION=${FFNX_CMAKEFMT_VERSION}"
      "-DFFNX_FORMAT_CHECK=${_ffnx_format_check}"
      -P
      "${CMAKE_CURRENT_LIST_DIR}/FormatSources.cmake"
    COMMENT "Running ${_ffnx_format_target} with clang-format and cmakefmt"
    VERBATIM)
endforeach()
