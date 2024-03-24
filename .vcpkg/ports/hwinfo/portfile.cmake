vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO julianxhokaxhiu/hwinfo
    REF b683e40cf44a44815cb23704d5c289e1a98d6f16
    SHA512 9cbe160c4c51c718122c3dc62ecf5bdf6853242712e146988a823bb7a7b6a14351e22fee840502522a8c62c09fb0e73057cec48fe95219e8e01fb7af76c96f29
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
