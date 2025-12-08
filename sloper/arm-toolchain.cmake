# to build, run: cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
# arm-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set cross-compiler

# full path to the buildroot toolchain
set(BUILDROOT_HOST_BIN "/ad/eng/courses/ec/ec535/bbb/buildroot-2021.02.1/output/host/bin")

# set the compiler paths directly
set(CMAKE_C_COMPILER "${BUILDROOT_HOST_BIN}/arm-linux-gcc")
set(CMAKE_CXX_COMPILER "${BUILDROOT_HOST_BIN}/arm-linux-g++")
set(CMAKE_CXX_LINKER "${BUILDROOT_HOST_BIN}/arm-linux-g++")

# make sure compilers exist
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "C compiler not found at: ${CMAKE_C_COMPILER}")
endif()
if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "C++ compiler not found at: ${CMAKE_CXX_COMPILER}")
endif()

# also set the linker explicitly
set(CMAKE_LINKER "${BUILDROOT_HOST_BIN}/arm-linux-ld")

set(CMAKE_SYSROOT /ad/eng/courses/ec/ec535/bbb/buildroot-2021.02.1/output/host/arm-buildroot-linux-gnueabihf/sysroot)

# CMAKE_FIND_ROOT_PATH
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")

set(CMAKE_PREFIX_PATH 
    "${CMAKE_SYSROOT}/usr"
    "${CMAKE_SYSROOT}/usr/lib/cmake"
    "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5"
    "${CMAKE_SYSROOT}/usr/lib64/cmake/Qt5"
)

# also explicitly set Qt5 location as CACHE variables
# also with FORCE to override any cached values
set(QT_DIR "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5" CACHE PATH "Qt5 CMake config directory" FORCE)
set(Qt5_DIR "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5" CACHE PATH "Qt5 CMake config directory" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# use BOTH to search in both sysroot and CMAKE_PREFIX_PATH
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)