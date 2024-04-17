vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF c02bca930fb3ebbfc0c3098b6a3ace188d8486c8
    SHA512 cbb734df915332b931f3db372981591cee7cde678f00a91ca03fce5f7bd72a00c7515e36db198bd6cce40c60b4ac727cdf1f3a5332a865e1f66afbb5e769cd7c
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
