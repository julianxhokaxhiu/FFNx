vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF e6ada31e6610580e14036b15ae230b08f802284e
    SHA512 838e79d1ed4d1f6f95617291d11aaea669ad4e7a2149915ed414e8f442366e9d214b2eb65655dfee8ffc88be1a8f45d547f4f5c62434b8bfad33fc7dbbfd617f
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")