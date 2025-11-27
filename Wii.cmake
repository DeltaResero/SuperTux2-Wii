# Wii.cmake - Toolchain file for devkitPPC
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)

# Detect devkitPPC environment
set(DEVKITPRO $ENV{DEVKITPRO})
set(DEVKITPPC $ENV{DEVKITPPC})

if(NOT DEVKITPPC)
    message(FATAL_ERROR "DEVKITPPC environment variable not set")
endif()

# Compiler settings
set(CMAKE_C_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-g++")
set(CMAKE_AR "${DEVKITPPC}/bin/powerpc-eabi-ar")
set(CMAKE_RANLIB "${DEVKITPPC}/bin/powerpc-eabi-ranlib")

# Wii Hardware Flags
# -mrvl: Revolution (Wii) architecture
# -mcpu=750: Broadway CPU
# -meabi: Embedded ABI
# -mhard-float: Hardware floating point
set(WII_FLAGS "-g -O2 -Wall -mrvl -mcpu=750 -meabi -mhard-float -D_WII_ -D__WII__ -DGEKKO")

set(CMAKE_C_FLAGS "${WII_FLAGS} -ffunction-sections -fdata-sections" CACHE STRING "C Flags")
set(CMAKE_CXX_FLAGS "${WII_FLAGS} -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions" CACHE STRING "C++ Flags")

# Linker Flags
set(CMAKE_EXE_LINKER_FLAGS "${WII_FLAGS} -Wl,--gc-sections" CACHE STRING "Linker Flags")

# Library Search Paths
set(CMAKE_FIND_ROOT_PATH ${DEVKITPRO}/libogc ${DEVKITPRO}/portlibs/wii)

# Don't look for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Look for libraries and headers in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Global Includes
include_directories(SYSTEM ${DEVKITPRO}/libogc/include)
include_directories(SYSTEM ${DEVKITPRO}/portlibs/wii/include)
include_directories(SYSTEM ${DEVKITPRO}/portlibs/wii/include/SDL2)

# Global Library Paths
link_directories(${DEVKITPRO}/libogc/lib/wii)
link_directories(${DEVKITPRO}/portlibs/wii/lib)
