# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages

set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "julianxhokaxhiu/SteamworksSDKCI"
    HEAD_REF master
    REF 1.61
    SHA512 a661ab4e8937e70869401dc903bdc297180be00577fa8f3caf98fbec45e02c04404d5f69823c89a5d7e7d7772145b7a15ec924b4a599bdacf2448724e92cc980
)

file(TO_NATIVE_PATH "${CURRENT_PACKAGES_DIR}/include/steamworkssdk" INCLUDE_PATH)
file(TO_NATIVE_PATH "${CURRENT_PACKAGES_DIR}/tools/steamworkssdk" TOOLS_PATH)

file(MAKE_DIRECTORY "${INCLUDE_PATH}")

file(GLOB HEADER_FILES "${SOURCE_PATH}/steamworks_sdk/public/steam/*.h")

file(COPY ${SOURCE_PATH}/steamworks_sdk/redistributable_bin/steam_api.dll DESTINATION ${TOOLS_PATH})
file(RENAME ${TOOLS_PATH}/steam_api.dll ${TOOLS_PATH}/FFNx_steam_api.dll)
file(COPY ${HEADER_FILES} DESTINATION ${INCLUDE_PATH})

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/STEAMWORKSSDKConfig.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/STEAMWORKSSDKConfig.cmake @ONLY)

################################################
### Redirect steam_api.dll to FFNx_steam_api.dll
################################################

set(EXPORTS_FILE "${CURRENT_BUILDTREES_DIR}/exports.txt")
set(EXPORTS_LOGFILE "${CURRENT_BUILDTREES_DIR}/exports.log")
set(DEF_FILE "${CURRENT_BUILDTREES_DIR}/FFNx_steam_api.def")
set(EXP_FILE "${CURRENT_BUILDTREES_DIR}/FFNx_steam_api.exp")
set(NEW_LIB_FILE "${CURRENT_BUILDTREES_DIR}/FFNx_steam_api.lib")

# Dumpbin to extract exports
execute_process(
  COMMAND dumpbin /EXPORTS ${SOURCE_PATH}/steamworks_sdk/redistributable_bin/steam_api.lib
  OUTPUT_FILE ${EXPORTS_FILE}
  ERROR_FILE ${EXPORTS_LOGFILE}
)

# Generate .def file using CMake script logic
file(WRITE ${DEF_FILE} "LIBRARY FFNx_steam_api.dll\nEXPORTS\n")
file(STRINGS ${EXPORTS_FILE} EXPORT_LINES REGEX "^                  .+$")
foreach(EXPORT_LINE ${EXPORT_LINES})
  string(REGEX MATCH "_(.+)" EXPORT_NAME ${EXPORT_LINE})
  if(CMAKE_MATCH_1)
    set(EXPORT_NAME ${CMAKE_MATCH_1})
    string(STRIP ${EXPORT_NAME} EXPORT_NAME)
    file(APPEND ${DEF_FILE} "${EXPORT_NAME}\n")
  endif()
endforeach()

# Create new .lib file using the .def file
execute_process(
  COMMAND lib /machine:x86 /def:${DEF_FILE} /out:${NEW_LIB_FILE}
  WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}
)

# Replace the original .lib file
file(COPY ${NEW_LIB_FILE} DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
file(COPY ${NEW_LIB_FILE} DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib)

# Post-install actions to clean up
file(REMOVE_RECURSE ${EXPORTS_FILE} ${EXPORTS_LOGFILE} ${DEF_FILE} ${NEW_LIB_FILE} ${EXP_FILE})
