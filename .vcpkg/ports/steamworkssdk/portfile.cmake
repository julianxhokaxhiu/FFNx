# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages

set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "julianxhokaxhiu/SteamworksSDKCI"
    HEAD_REF master
    REF d320d9566179684d4fc597e151f50ad0beaaa4d0
    SHA512 dc7d4ee95353737fa38b68204755ec3b0a7a2216cc60af6da20b491ba729b2a8003a6511b34690cb254a0db91c79b8df588a40e7e75aed9d00d10c35eb221583
)

file(TO_NATIVE_PATH "${CURRENT_PACKAGES_DIR}/include/steamworkssdk" INCLUDE_PATH)
file(TO_NATIVE_PATH "${CURRENT_PACKAGES_DIR}/tools/steamworkssdk" TOOLS_PATH)

file(MAKE_DIRECTORY
    "${INCLUDE_PATH}"
)

file(GLOB 
    HEADER_FILES
    "${SOURCE_PATH}/steamworks_sdk/public/steam/*.h"
)

file(COPY ${SOURCE_PATH}/steamworks_sdk/redistributable_bin/steam_api.lib DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
file(COPY ${SOURCE_PATH}/steamworks_sdk/redistributable_bin/steam_api.lib DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib)
file(COPY ${SOURCE_PATH}/steamworks_sdk/redistributable_bin/steam_api.dll DESTINATION ${TOOLS_PATH})
file(COPY ${HEADER_FILES} DESTINATION ${INCLUDE_PATH})

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/STEAMWORKSSDKConfig.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/STEAMWORKSSDKConfig.cmake @ONLY)
