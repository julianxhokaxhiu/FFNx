# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Checkout dependencies

vcpkg_from_github(
    OUT_SOURCE_PATH HE_SOURCE_DIR
    REPO "julianxhokaxhiu/highly_experimental"
    HEAD_REF master
    REF 8a045a8619116ce2b4c11387f527cc48636ef7a9
    SHA512 12f948cbac27ffb0807201604b2dd38467758d0f98af30b6ad3d433bc0e2e111eea1d970bf1c8944c1edb261f9e4f5e1b0344a902383d16182393eb521e6894b
)

vcpkg_from_github(
    OUT_SOURCE_PATH PSFLIB_SOURCE_DIR
    REPO "julianxhokaxhiu/psflib"
    HEAD_REF master
    REF 93d14f05f5943ca5437378a731be60f8525366be
    SHA512 a63ae8bfa3ea0b45fe957eebb5a0d0c4cb829e05c6255525a88a019c13f4bf1900234d0894be2bf75f5ecfd8f1783b188576a534f1e192e4cfc01aed9e041b86
)

# Checkout this project

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_DIR
    REPO "julianxhokaxhiu/openpsf"
    HEAD_REF master
    REF 9b832c44ab57084f5e96aa6b78eb3a68e77e0cd9
    SHA512 77fe1bd888cd434f2fe61af972b6d406341b30571e4d6db5b3ad413de156c4e8b29068a9826c093b707bbd0e7cf12e658d515ee4cb02c98fb2e2a7e905ae8bc8
)

# Move dependencies inside the project directory
file(RENAME ${HE_SOURCE_DIR} "${SOURCE_DIR}/highly_experimental")
file(RENAME ${PSFLIB_SOURCE_DIR} "${SOURCE_DIR}/psflib")

# Run MSBuild

vcpkg_install_msbuild(
    SOURCE_PATH "${SOURCE_DIR}"
    PROJECT_SUBPATH "openpsf.sln"
    LICENSE_SUBPATH "LICENSE"
    INCLUDES_SUBPATH "include"
    USE_VCPKG_INTEGRATION
)

# Copy dependencies headers
file(INSTALL "${SOURCE_DIR}/highly_experimental/include/highly_experimental" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
file(INSTALL "${SOURCE_DIR}/psflib/include/psflib" DESTINATION "${CURRENT_PACKAGES_DIR}/include")

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/FindOPENPSF.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/FindOPENPSF.cmake @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
