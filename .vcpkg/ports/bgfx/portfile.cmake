# For a list of common variables see https://github.com/microsoft/vcpkg/blob/master/docs/maintainers/vcpkg_common_definitions.md

# Download source packages
# (bgfx requires bx and bimg source for building)

vcpkg_from_github(OUT_SOURCE_PATH BX_SOURCE_DIR
    REPO "julianxhokaxhiu/bx"
    HEAD_REF master
    REF c4bbf8f5afd6731ed8b88dbb6e3dd78d138f3940
    SHA512 8020ed3ba4353969b5a6fc7db05f8c8fb66d3489472a67eb676ec284a54b69a52b445af96e57b0a90cf5ee13ed785cafa6467010b02302d45be36d2c5ddf6aae
)

vcpkg_from_github(OUT_SOURCE_PATH BIMG_SOURCE_DIR
    REPO "julianxhokaxhiu/bimg"
    HEAD_REF master
    REF 77bebb28e12956bfced82bb2cf7c19ac5735b6c8
    SHA512 e6492cb5dde4a0cd1707a947de3142d18d4b41faf9f728748259655e4c862fec95fa8363816ec94f944812c8c487d16ec774ba4b33f8c8e771f76631d9aa0fb6
)

vcpkg_from_github(OUT_SOURCE_PATH SOURCE_DIR
    REPO "julianxhokaxhiu/bgfx"
    HEAD_REF master
    REF d80f4c2647c5b80dd0e9aebf74cad2e0bc35ea5c
    SHA512 96328d40b43c881f2295a8aad817e2933a439e51cc0909924675752b06eab1386cdc68802bfe5ff22a4acefd43e66cadf8493f10816b30859a44b402c1fc98de
)

# Move bx source inside bgfx source tree
set(BX_DIR ${SOURCE_DIR}/.bx)
file(RENAME ${BX_SOURCE_DIR} "${BX_DIR}")
set(ENV{BX_DIR} ${BX_DIR})

# Move bimg source inside bgfx source tree
set(BIMG_DIR ${SOURCE_DIR}/.bimg)
file(RENAME ${BIMG_SOURCE_DIR} "${BIMG_DIR}")
set(ENV{BIMG_DIR} ${BIMG_DIR})

# Set custom BGFX configuration
math(EXPR BGFX_CONFIG_DYNAMIC_INDEX_BUFFER_SIZE "10<<20")
math(EXPR BGFX_CONFIG_DYNAMIC_VERTEX_BUFFER_SIZE "30<<20")
math(EXPR BGFX_CONFIG_MAX_RECT_CACHE "8<<10")
math(EXPR BGFX_CONFIG_RENDERER_VULKAN_DESCRIPTOR_SETS_PER_POOL "10240")
set(ENV{BGFX_CONFIG} "DEBUG=1:PREFER_DISCRETE_GPU=0:DYNAMIC_INDEX_BUFFER_SIZE=${BGFX_CONFIG_DYNAMIC_INDEX_BUFFER_SIZE}:DYNAMIC_VERTEX_BUFFER_SIZE=${BGFX_CONFIG_DYNAMIC_VERTEX_BUFFER_SIZE}:MAX_RECT_CACHE=${BGFX_CONFIG_MAX_RECT_CACHE}:RENDERER_VULKAN_DESCRIPTOR_SETS_PER_POOL=${BGFX_CONFIG_RENDERER_VULKAN_DESCRIPTOR_SETS_PER_POOL}")

# Set up GENie (custom project generator)
set(GENIE_OPTIONS --with-tools)

if(VCPKG_CRT_LINKAGE STREQUAL dynamic)
    set(GENIE_OPTIONS ${GENIE_OPTIONS} --with-dynamic-runtime)
endif()
if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
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
    set(PROJ_SLN_EXT "sln")
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
        set(PROJ_SLN_EXT "slnx")
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
    set(GENIE "${BX_DIR}/tools/bin/windows/genie.exe")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(GENIE "${BX_DIR}/tools/bin/darwin/genie")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(GENIE "${BX_DIR}/tools/bin/linux/genie")
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
    if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
        set(PROJ bgfx-shared-lib)
    else()
        set(PROJ bgfx)
    endif()
    vcpkg_configure_cmake(
        SOURCE_PATH "${SOURCE_DIR}/.build/projects/${PROJ_FOLDER}"
        PREFER_NINJA
        OPTIONS_RELEASE -DCMAKE_BUILD_TYPE=Release
        OPTIONS_DEBUG -DCMAKE_BUILD_TYPE=Debug
    )
    vcpkg_install_cmake(TARGET ${PROJ}/all)
    file(INSTALL "${SOURCE_DIR}/include/bgfx" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
    file(GLOB instfiles
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/${PROJ}/*.a"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/${PROJ}/*.so"
    )
    file(INSTALL ${instfiles} DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
    file(GLOB instfiles
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/${PROJ}/*.a"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/${PROJ}/*.so"
    )
    file(INSTALL ${instfiles} DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
    file(INSTALL "${SOURCE_DIR}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME "copyright")
else()
    vcpkg_msbuild_install(
        SOURCE_PATH "${SOURCE_DIR}"
        PROJECT_SUBPATH ".build/projects/${PROJ_FOLDER}/bgfx.${PROJ_SLN_EXT}"
    )
    file(INSTALL "${SOURCE_DIR}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME "copyright")
    file(INSTALL "${SOURCE_DIR}/include/" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
    # Install shader include file
    file(INSTALL "${SOURCE_DIR}/src/bgfx_shader.sh" DESTINATION "${CURRENT_PACKAGES_DIR}/include/bgfx" )
    # Remove redundant files
    foreach(a bx bimg bimg_decode bimg_encode)
        foreach(b Debug Release)
            foreach(c lib pdb)
                if(b STREQUAL Debug)
                    file(REMOVE "${CURRENT_PACKAGES_DIR}/debug/lib/${a}${b}.${c}")
                else()
                    file(REMOVE "${CURRENT_PACKAGES_DIR}/lib/${a}${b}.${c}")
                endif()
            endforeach()
        endforeach()
    endforeach()
endif()

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
configure_file(${CMAKE_CURRENT_LIST_DIR}/FindBGFX.cmake.in ${CURRENT_PACKAGES_DIR}/share/${PORT}/FindBGFX.cmake @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
