# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

# Download source

vcpkg_from_github(OUT_SOURCE_PATH SOURCE_DIR
    REPO "julianxhokaxhiu/bx"
    HEAD_REF master
    REF c99974204c15405b69647d5097af1102e6210c91
    SHA512 94bb2d2f2cc269404032e7ead02419939453b15d02e898dcff9ddb5b0d5ab5be3af253214074e1d5c62b3961d1c80bb9f966c31094e183f028d66037b6c38f94
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
    elseif(VCPKG_PLATFORM_TOOLSET STREQUAL "v145")
        set(GENIE_ACTION vs2026)
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
