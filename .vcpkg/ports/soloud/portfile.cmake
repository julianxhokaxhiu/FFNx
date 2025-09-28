# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages

vcpkg_from_github(OUT_SOURCE_PATH SOURCE_DIR
    REPO "julianxhokaxhiu/soloud"
    HEAD_REF master
    REF bbb2e0b24dc61a1983f32d962bcf7b25be8deeed
    SHA512 c17bfe29334e4c2b0e7e5c0fc41eac5146fa29a15d024a6e74e178c6e4a0c75b9aef566a485a32397f43699f6ec7fb7d75f7e63d79bee34360dd0c460d128abc
)

# Set up GENie (custom project generator)

set(SOLOUD_PROJNAME Static)
set(GENIE_OPTIONS --with-miniaudio-only)

if(VCPKG_CRT_LINKAGE STREQUAL dynamic)
    set(SOLOUD_PROJNAME Dynamic)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --with-dynamic-runtime)
endif()
if(VCPKG_LIBRARY_LINKAGE STREQUAL dynamic)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --with-shared-lib)
endif()

if(VCPKG_TARGET_ARCHITECTURE STREQUAL x86)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --platform=x32)
elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL x64)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --platform=x64)
elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL arm OR VCPKG_TARGET_ARCHITECTURE STREQUAL arm64)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --platform=ARM)
else()
    message(WARNING "Architecture may be not supported: ${VCPKG_TARGET_ARCHITECTURE}")
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --platform=${VCPKG_TARGET_ARCHITECTURE})
endif()

if(TARGET_TRIPLET MATCHES osx)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --os=macosx)
elseif(TARGET_TRIPLET MATCHES linux)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --os=linux)
elseif(TARGET_TRIPLET MATCHES windows)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --os=windows)
elseif(TARGET_TRIPLET MATCHES uwp)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --vs=winstore100)
endif()

# GENie does not allow cmake+msvc, so we use msbuild in windows
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    if(VCPKG_PLATFORM_TOOLSET STREQUAL "v140")
        set(GENIE_ACTION vs2015)
    elseif(VCPKG_PLATFORM_TOOLSET STREQUAL "v141")
        set(GENIE_ACTION vs2017)
    elseif(VCPKG_PLATFORM_TOOLSET STREQUAL "v142")
        set(GENIE_ACTION vs2019)
    elseif(VCPKG_PLATFORM_TOOLSET STREQUAL "v143")
        set(GENIE_ACTION vs2022)
    else()
        message(FATAL_ERROR "Unsupported Visual Studio toolset: ${VCPKG_PLATFORM_TOOLSET}")
    endif()
    set(PROJ_FOLDER ${GENIE_ACTION})
    if(TARGET_TRIPLET MATCHES uwp)
        set(PROJ_FOLDER ${PROJ_FOLDER}-winstore100)
    endif()
else()
    set(GENIE_ACTION cmake)
    set(PROJ_FOLDER ${GENIE_ACTION})
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(GENIE "${CURRENT_INSTALLED_DIR}/tools/bx/genie.exe")
else()
    message(FATAL_ERROR "Unsupported host platform: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

# Run GENie

vcpkg_execute_required_process(
    COMMAND ${GENIE} ${GENIE_OPTIONS} ${GENIE_ACTION}
    WORKING_DIRECTORY "${SOURCE_DIR}/build"
    LOGNAME "genie-${TARGET_TRIPLET}"
)

# Run MSBuild

vcpkg_msbuild_install(
    SOURCE_PATH "${SOURCE_DIR}"
    PROJECT_SUBPATH "build/${GENIE_ACTION}/SoLoud${SOLOUD_PROJNAME}.vcxproj"
)
file(INSTALL "${SOURCE_DIR}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME "copyright")
file(INSTALL "${SOURCE_DIR}/include/" DESTINATION "${CURRENT_PACKAGES_DIR}/include")

# Copy cmake configuration files
configure_file(${CMAKE_CURRENT_LIST_DIR}/FindSOLOUD.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/FindSOLOUD.cmake @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
