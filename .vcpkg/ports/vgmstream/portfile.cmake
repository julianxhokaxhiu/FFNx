# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages
# (bgfx requires bx and bimg source for building)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "vgmstream/vgmstream"
    HEAD_REF master
    REF 9d47eded30c94ce6242b6e63c0624524fa637826
    SHA512 9eff06cd8673b491b0881f95afecce0bef2a17c207c3fc94f5f56672b9600bf015faccbb20c19a53e09993eef6ccf0cc7c8db93d41c3131f05b4146ad8c8fc34
    PATCHES cmake.patch
)

SET(USE_FFMPEG OFF)
SET(USE_MPEG OFF)
SET(USE_VORBIS OFF)

if("ffmpeg" IN_LIST FEATURES)
    SET(USE_FFMPEG ON)
endif()

if("mpg123" IN_LIST FEATURES)
    SET(USE_MPEG ON)
endif()

if("vorbis" IN_LIST FEATURES)
    SET(USE_VORBIS ON)
endif()

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DUSE_ATRAC9=OFF
        -DUSE_CELT=OFF
        -DUSE_FFMPEG=${USE_FFMPEG}
        -DAVCODEC_VERSION=57
        -DAVUTIL_VERSION=55
        -DAVFORMAT_VERSION=57
        -DSWRESAMPLE_VERSION=2
        -DUSE_G719=OFF
        -DUSE_G7221=ON
        -DUSE_MAIATRAC3PLUS=OFF
        -DUSE_MPEG=${USE_MPEG}
        -DUSE_VORBIS=${USE_VORBIS}
        -DBUILD_AUDACIOUS=OFF
        -DBUILD_CLI=OFF
        -DBUILD_FB2K=OFF
        -DBUILD_XMPLAY=OFF
        -DBUILD_WINAMP=OFF
    OPTIONS_DEBUG
        -DSKIP_INSTALL_HEADERS=ON
)

vcpkg_install_cmake()

vcpkg_copy_pdbs()

file(INSTALL ${SOURCE_PATH}/COPYING DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/FindVGMSTREAM.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/FindVGMSTREAM.cmake @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
