set(FFNX_CMAKEFMT_VERSION "2.4.0")
set(FFNX_CMAKEFMT_SHA256 "9509f7253e3f5eaf235dd6f4d8f477026bd143d31b81e99b1ab9414e9a2b81d3")
set(_ffnx_cmakefmt_tools_dir "${CMAKE_BINARY_DIR}/tools")
set(_ffnx_cmakefmt_dir "${_ffnx_cmakefmt_tools_dir}/cmakefmt")
if(FFNX_CMAKEFMT_EXECUTABLE STREQUAL "${_ffnx_cmakefmt_tools_dir}/cmakefmt-${FFNX_CMAKEFMT_VERSION}-x86_64-pc-windows-msvc/cmakefmt.exe")
  set(FFNX_CMAKEFMT_EXECUTABLE "${_ffnx_cmakefmt_dir}/cmakefmt.exe" CACHE FILEPATH "Path to the pinned cmakefmt executable" FORCE)
endif()
set(FFNX_CMAKEFMT_EXECUTABLE "${_ffnx_cmakefmt_dir}/cmakefmt.exe" CACHE FILEPATH "Path to the pinned cmakefmt executable")

set(_ffnx_cmakefmt_download ON)
if(EXISTS "${FFNX_CMAKEFMT_EXECUTABLE}")
  execute_process(
    COMMAND "${FFNX_CMAKEFMT_EXECUTABLE}" --version
    OUTPUT_VARIABLE _ffnx_cmakefmt_version_output OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)
  if(NOT _ffnx_cmakefmt_version_output MATCHES "^cmakefmt ([0-9]+\\.[0-9]+\\.[0-9]+)")
    message(FATAL_ERROR "Unexpected cmakefmt version output: ${_ffnx_cmakefmt_version_output}")
  endif()
  if(CMAKE_MATCH_1 STREQUAL FFNX_CMAKEFMT_VERSION)
    set(_ffnx_cmakefmt_download OFF)
  elseif(NOT FFNX_CMAKEFMT_EXECUTABLE STREQUAL "${_ffnx_cmakefmt_dir}/cmakefmt.exe")
    message(FATAL_ERROR "FFNX_CMAKEFMT_EXECUTABLE must provide cmakefmt ${FFNX_CMAKEFMT_VERSION}: ${_ffnx_cmakefmt_version_output}")
  endif()
endif()

if(_ffnx_cmakefmt_download)
  if(NOT CMAKE_HOST_WIN32)
    message(FATAL_ERROR "Automatic cmakefmt installation requires Windows. Set FFNX_CMAKEFMT_EXECUTABLE to cmakefmt ${FFNX_CMAKEFMT_VERSION}.")
  endif()
  if(NOT FFNX_CMAKEFMT_EXECUTABLE STREQUAL "${_ffnx_cmakefmt_dir}/cmakefmt.exe")
    message(FATAL_ERROR "FFNX_CMAKEFMT_EXECUTABLE does not exist: ${FFNX_CMAKEFMT_EXECUTABLE}")
  endif()

  set(_ffnx_cmakefmt_archive "${_ffnx_cmakefmt_dir}/cmakefmt.zip")
  set(_ffnx_cmakefmt_extract_dir "${_ffnx_cmakefmt_dir}/archive")
  file(MAKE_DIRECTORY "${_ffnx_cmakefmt_dir}")
  message(STATUS "Downloading cmakefmt ${FFNX_CMAKEFMT_VERSION}")
  file(
    DOWNLOAD
    "https://github.com/cmakefmt/cmakefmt/releases/download/v${FFNX_CMAKEFMT_VERSION}/cmakefmt-${FFNX_CMAKEFMT_VERSION}-x86_64-pc-windows-msvc.zip"
    "${_ffnx_cmakefmt_archive}"
    EXPECTED_HASH
    "SHA256=${FFNX_CMAKEFMT_SHA256}"
    TLS_VERIFY
    ON
    STATUS
    _ffnx_cmakefmt_download_status)
  list(GET _ffnx_cmakefmt_download_status 0 _ffnx_cmakefmt_download_code)
  if(NOT _ffnx_cmakefmt_download_code EQUAL 0)
    message(FATAL_ERROR "cmakefmt download failed: ${_ffnx_cmakefmt_download_status}")
  endif()
  file(ARCHIVE_EXTRACT INPUT "${_ffnx_cmakefmt_archive}" DESTINATION "${_ffnx_cmakefmt_extract_dir}")
  set(_ffnx_cmakefmt_extracted_executable "${_ffnx_cmakefmt_extract_dir}/cmakefmt-${FFNX_CMAKEFMT_VERSION}-x86_64-pc-windows-msvc/cmakefmt.exe")
  if(NOT EXISTS "${_ffnx_cmakefmt_extracted_executable}")
    message(FATAL_ERROR "cmakefmt archive did not contain the expected executable: ${_ffnx_cmakefmt_extracted_executable}")
  endif()
  file(COPY_FILE "${_ffnx_cmakefmt_extracted_executable}" "${FFNX_CMAKEFMT_EXECUTABLE}")
  file(REMOVE "${_ffnx_cmakefmt_archive}")
  file(REMOVE_RECURSE "${_ffnx_cmakefmt_extract_dir}")
endif()
