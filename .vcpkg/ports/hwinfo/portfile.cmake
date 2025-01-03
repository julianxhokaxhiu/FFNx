vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO lfreist/hwinfo
    REF 821a23ac6f61e8baa539c5b4b0aeb44da9cb93da
    SHA512 4fef9029b54ec173bf147bd40ee38d27b7fb7844e746d9c416de2c5e28c2a1c79b11392a941696d9437c5d1edf006a8d709c0a3ea2baf1245918e12ab47e9866
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