# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

# Download source

vcpkg_from_github(OUT_SOURCE_PATH SOURCE_DIR
    REPO "julianxhokaxhiu/bx"
    HEAD_REF master
    REF 2c17bea5d83c14121a3bfb18840651803c2e2623
    SHA512 265120ddc66dea6f5d43614a10cb2edbc116f542ca0f432a3a017498766d3cab9f9adb3221311f3738626cad96454390fef7e70ee34e1d59cf9cd3d921fda59c
)

# Set up GENie (custom project generator)

if(VCPKG_CRT_LINKAGE STREQUAL dynamic)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --with-dynamic-runtime)
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
    set(GENIE "${SOURCE_DIR}/tools/bin/windows/genie.exe")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(GENIE "${SOURCE_DIR}/tools/bin/darwin/genie")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(GENIE "${SOURCE_DIR}/tools/bin/linux/genie")
else()
    message(FATAL_ERROR "Unsupported host platform: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

# Run GENie

vcpkg_execute_required_process(
    COMMAND ${GENIE} ${GENIE_OPTIONS} ${GENIE_ACTION}
    WORKING_DIRECTORY "${SOURCE_DIR}"
    LOGNAME "genie-${TARGET_TRIPLET}"
)

if(GENIE_ACTION STREQUAL cmake)
    # Run CMake
    vcpkg_configure_cmake(
        SOURCE_PATH "${SOURCE_DIR}/.build/projects/${PROJ_FOLDER}"
        PREFER_NINJA
        OPTIONS_RELEASE -DCMAKE_BUILD_TYPE=Release
        OPTIONS_DEBUG -DCMAKE_BUILD_TYPE=Debug
    )
    vcpkg_install_cmake(TARGET bx/all)
    # GENie does not generate an install target, so we install explicitly
    file(INSTALL
        "${SOURCE_DIR}/include/bx"
        "${SOURCE_DIR}/include/compat"
        "${SOURCE_DIR}/include/tinystl"
        DESTINATION "${CURRENT_PACKAGES_DIR}/include")
    file(GLOB instfiles
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/bx/*.a"
    )
    file(INSTALL ${instfiles} DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
    file(GLOB instfiles
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/bx/*.a"
    )
    file(INSTALL ${instfiles} DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
    file(INSTALL "${SOURCE_DIR}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
else()
    # Run MSBuild
    vcpkg_msbuild_install(
        SOURCE_PATH "${SOURCE_DIR}"
        PROJECT_SUBPATH ".build/projects/${PROJ_FOLDER}/bx.vcxproj"
    )
    file(INSTALL "${SOURCE_DIR}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME "copyright")
    file(INSTALL "${SOURCE_DIR}/include/" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
endif()

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
configure_file(${CMAKE_CURRENT_LIST_DIR}/FindBX.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/FindBX.cmake @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
