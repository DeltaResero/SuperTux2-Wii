# cmake/Wii.cmake
# CMake toolchain file for Nintendo Wii (devkitPPC)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)

# Ensure DEVKITPPC is set
if(NOT DEFINED ENV{DEVKITPPC})
    message(FATAL_ERROR "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif()

if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif()

set(DEVKITPPC $ENV{DEVKITPPC})
set(DEVKITPRO $ENV{DEVKITPRO})

# Set compilers
set(CMAKE_C_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-g++")
set(CMAKE_AR "${DEVKITPPC}/bin/powerpc-eabi-ar" CACHE FILEPATH "Archiver")
set(CMAKE_RANLIB "${DEVKITPPC}/bin/powerpc-eabi-ranlib" CACHE FILEPATH "Ranlib")
set(CMAKE_STRIP "${DEVKITPPC}/bin/powerpc-eabi-strip" CACHE FILEPATH "Strip")

# Set the sysroot
set(CMAKE_FIND_ROOT_PATH ${DEVKITPPC} ${DEVKITPRO}/portlibs/wii ${DEVKITPRO}/portlibs/ppc)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Wii architecture flags
set(WII_ARCH_FLAGS "-mrvl -mcpu=750 -meabi -mhard-float")

# Wii compiler flags
set(CMAKE_C_FLAGS_INIT "${WII_ARCH_FLAGS} -D_WII_ -DGEKKO")
set(CMAKE_CXX_FLAGS_INIT "${WII_ARCH_FLAGS} -D_WII_ -DGEKKO")

# Optimization flags
set(CMAKE_C_FLAGS_RELEASE "-O3" CACHE STRING "C Release flags")
set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "CXX Release flags")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-L${DEVKITPRO}/libogc/lib/wii -L${DEVKITPRO}/portlibs/wii/lib -L${DEVKITPRO}/portlibs/ppc/lib")

# Set library paths
set(LIBOGC_INC "${DEVKITPRO}/libogc/include")
set(PORTLIBS "${DEVKITPRO}/portlibs/wii")
set(PORTLIBS_PPC "${DEVKITPRO}/portlibs/ppc")

# Include directories
include_directories(SYSTEM
    ${LIBOGC_INC}
    ${PORTLIBS}/include
    ${PORTLIBS_PPC}/include
    ${PORTLIBS}/include/SDL2
    ${PORTLIBS}/include/GL
)

# Force static linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Disable tests
set(BUILD_TESTS OFF CACHE BOOL "Build test cases" FORCE)

# Enable OpenGL (via OpenGX wrapper)
set(ENABLE_OPENGL ON CACHE BOOL "Enable OpenGL support" FORCE)
set(ENABLE_OPENAL OFF CACHE BOOL "Enable OpenAL support" FORCE)
