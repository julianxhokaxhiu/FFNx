vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF 864033eddbd941aafee7237d8c747f3e90e8ffba
    SHA512 72cf63860b2009d62128e85f1fa63f2c8f3a6d8892fcf5713ee479b5d83c0e5639a2632c0fc3449e6e649115c25f69f4b381abe52ca5b6c55c0fc585535560b9
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