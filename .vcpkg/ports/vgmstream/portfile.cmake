# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages
# (bgfx requires bx and bimg source for building)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "vgmstream/vgmstream"
    HEAD_REF master
    REF 0dddcfc7ea39b47f20d7e75afd31298f516f9341
    SHA512 0126ab48188f0fabde15bbc9505d68d22cb97a2c005fd69721f88fe2fed816f72aa4de255e5b209770a5853b397d811aa707bd37e28c62613377a062b7959296
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
