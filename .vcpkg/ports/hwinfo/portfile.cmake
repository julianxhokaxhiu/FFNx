vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO lfreist/hwinfo
    REF 3d234f4799f9eb7651f9853bd3fd17456b191a58
    SHA512 f109449ffb678810383ee5f39d6ba098b96779ace538f0301833c5b2f168a119ca325a47df2353a783b1cdb47e0c282b18bb578587dd79568e7defe115b27350
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
