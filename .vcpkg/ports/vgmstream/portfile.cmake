# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages
# (bgfx requires bx and bimg source for building)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "vgmstream/vgmstream"
    HEAD_REF master
    REF 26e35a029f93a6143d64d0e3a9f78cc3e6f099be
    SHA512 ccd29ac4f2083a4a7c7b027522e7aea080edfb52856b4b604b5e5a66b61e27e919c45bf98df2866dd424237f8cfc0afed74d5e8fb098206ded3df4fba78874e0
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
    PREFER_NINJA
    OPTIONS
        -DUSE_ATRAC9=OFF
        -DUSE_CELT=OFF
        -DUSE_FFMPEG=${USE_FFMPEG}
        -DUSE_FDKAAC=OFF
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
