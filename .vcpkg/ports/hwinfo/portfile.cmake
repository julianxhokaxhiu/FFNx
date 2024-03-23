vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF d03b2282d0b10444186049d68ccf540512113de7
    SHA512 adf2c882e6880cddab93b6f486f274606af0e6d0d12daf74b7a374d5174323054b6055d8e9c6eee53c6b8c60191e1155bcabae3e39c7068f44e5bd04513722d0
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DNO_OCL=TRUE # disable OpenCL usage
    MAYBE_UNUSED_VARIABLES
        NO_OCL
)
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/hwinfoConfig.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/hwinfoConfig.cmake @ONLY)
