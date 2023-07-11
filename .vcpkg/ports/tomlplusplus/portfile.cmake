vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO marzer/tomlplusplus
    REF v3.3.0
    SHA512 6ab2de83b7fc44de40e58a47c28a9507bf7c50fa9b08925b5a6d48958868a86e6790aff684d29ceb50ad18905e3832840719e1b7bfec3b8a0c00b15bb0f70f38
    HEAD_REF master
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
)

vcpkg_install_cmake()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/tomlplusplus)
vcpkg_fixup_pkgconfig()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib")
