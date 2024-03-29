vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO lfreist/hwinfo
    REF 9ba5dfecb8fb47878c51a18b9b64cf3b5184e7ee
    SHA512 85346f0e1271bd85724946e0ffb5a3b50bdfb3e597519735641cc5656aae07853a7366019867bbe815157a603d0dd7e2290b1a7ae236a1890abb3ea3ff3232c0
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
