include(ExternalProject)

ExternalProject_Add(
    libpng
    URL                 "https://github.com/glennrp/libpng/archive/v1.6.37.tar.gz"
    URL_HASH            "SHA256=ca74a0dace179a8422187671aee97dd3892b53e168627145271cad5b5ac81307"
	INSTALL_DIR			"${CMAKE_BINARY_DIR}/vendor"
	PATCH_COMMAND       "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/third_party/libpng/CMakeLists.txt.in" "<SOURCE_DIR>/CMakeLists.txt"
	CMAKE_ARGS
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_PREFIX_PATH=<INSTALL_DIR>"
		"-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
	DEPENDS
	ffmpeg # it depends on zlib, which comes precompiled via ffmpeg
)
