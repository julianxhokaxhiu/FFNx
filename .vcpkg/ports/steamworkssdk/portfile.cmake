# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages

set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "julianxhokaxhiu/SteamworksSDKCI"
    HEAD_REF master
    REF 5889ae0b4276c19629c8a6623c14caee2f55e4d6
    SHA512 54e57767b4463ec978d2164ad4384de9eef47a903853ba207b6c89a06bc75f91cc60cc41053c72f869eb369fac833708e13b7dd8fafbf17bf1e2908b847a9984
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
