# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages

set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "julianxhokaxhiu/SteamworksSDKCI"
    HEAD_REF master
    REF 06d15de207e51f2acd7040fbc99b9cb8652ab4b1
    SHA512 f36eb67aa0ad2b6faefdf2063c1aa494c46ae75c267f0bf65d8afbb2045190dc71d418eaa15b9f5a6c249edd6f00facce724f65a9231e34b30b465201871418a
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
