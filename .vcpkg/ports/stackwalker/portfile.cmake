# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages
# (bgfx requires bx and bimg source for building)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "JochenKalmbach/StackWalker"
    HEAD_REF master
    REF 5b0df7a4db8896f6b6dc45d36e383c52577e3c6b
    SHA512 4a338e886c09f4350eaf46be6c4ca04b3be5e7b7909241047a72f2972af7bc7ad750272204d96869763ba251ca5cd5f4212c65628e35b18217970ae059545382
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
)

vcpkg_install_cmake()

vcpkg_copy_pdbs()

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/FindSTACKWALKER.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/FindSTACKWALKER.cmake @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
