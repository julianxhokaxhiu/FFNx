vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF a39f4ed9a148d3547a237dd49fa49db6a3c903c9
    SHA512 1d66efb7e3ed374f54e08992fe96d96271e01b5beffc8f80ef7aefe1e9cc7a6c6b588d00e9c434a84a74ff4e5838bce0fd407d36ce307a35cef04cf9a2d390ab
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(
    CONFIG_PATH lib/cmake/hwinfo
    PACKAGE_NAME lfreist-hwinfo
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")