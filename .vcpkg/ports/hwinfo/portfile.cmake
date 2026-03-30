vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF 431605c5d673c4f709505d50e1a4a20d4762406c
    SHA512 0f4b32846a0cd7f31f65423fba5dd1c01da674d0ac82522984b3ea682619520d5bf9691711ad69cdec84c191795c924d5e5082100f37135b774daac633ded25a
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