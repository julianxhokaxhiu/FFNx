vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF b11dc2775a0138f4636c6c179689e788d5d6ee71
    SHA512 0d5ba7c39252afd9aa33d79f7f53ac6b6e27c679e9f6f440000169396a3382142f07b360415c4a58a53c1126536a40d3775e658c30c5e2b4a59eafff21a1a352
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DHWINFO_SHARED=OFF
        -DHWINFO_STATIC=ON
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(
    CONFIG_PATH lib/cmake/hwinfo
    PACKAGE_NAME lfreist-hwinfo
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")