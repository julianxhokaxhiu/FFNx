vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF 9d12c96041b9a65485202a8082528de78d839d38
    SHA512 591405c081f536c1d0183b1cd3c3fea559696634d65b710cb59b83234f6eeec4f0d1d74862005927db181a097264c34775439cde31fdfe46399698cb1ef80a62
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