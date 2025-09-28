vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF 196c4a7313be90b49d1ef899998c32d462075e1d
    SHA512 638e09932bde83381baea9c311c66a573cf63f517a94aab2205eb6f69046437df5fbbb712a389205a4e81f6109fa1dc28d3bb4da432aabb529bc6a6f4edc9fd3
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