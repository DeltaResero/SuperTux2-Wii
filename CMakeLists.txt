#
# SuperTux - root build script
# Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#


#
# INSTRUCTIONS:
# -------------
#
# Create a directory build/ and change to it. Run
#
#   cmake ..
#
# This creates a set of Makefiles to build the project. Run
#
#   make
#


### CMake configuration

# The upper bound of the range opts in to the current policy behaviour
# without raising the version a builder has to have installed.
CMAKE_MINIMUM_REQUIRED(VERSION 3.16...4.0)

## Project name to use as command prefix.

PROJECT(SUPERTUX)

## Language standard. CMake reads these at target-creation time, so they have
## to stay above the first ADD_LIBRARY/ADD_EXECUTABLE in this file.

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

## Build type, the release one unless another is asked for. Generators that
## choose per build rather than per tree set their own.

IF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
ENDIF()
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             Debug Release RelWithDebInfo MinSizeRel)

## Compiler cache, asked for rather than assumed.

OPTION(ENABLE_CCACHE "Compile through ccache" OFF)
IF(ENABLE_CCACHE)
  FIND_PROGRAM(CCACHE_EXECUTABLE ccache)
  IF(CCACHE_EXECUTABLE)
    MESSAGE(STATUS "Compiling through ${CCACHE_EXECUTABLE}")
    SET(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
    SET(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
  ELSE()
    MESSAGE(WARNING "ENABLE_CCACHE is on but no ccache was found; compiling without it")
  ENDIF()
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake)

SET(BUILD_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
SET(BUILD_CONFIG_DATA_DIR "${CMAKE_BINARY_DIR}/data")
## Add definitions
IF(CMAKE_BUILD_TYPE MATCHES Release)
  ADD_DEFINITIONS(-DRELEASE)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DDEBUG)
ENDIF()

SET(SUPERTUX_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
## These variables enable MSVC to find libraries located in "dependencies{32|64}"
IF(WIN32)
  ## Store path of dependecy folder
  SET(DEPENDENCY_FOLDER "${PROJECT_SOURCE_DIR}/dependencies")

  ## To test if the host (not the build) is x64:
  ## "$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "AMD64"
  IF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    SET(WIN64 ON)
    SET(DEPENDENCY_FOLDER "${DEPENDENCY_FOLDER}64")
  ELSE(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    SET(WIN64 OFF)
    SET(DEPENDENCY_FOLDER "${DEPENDENCY_FOLDER}32")
  ENDIF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

  IF(WIN64)
    SET(SUPERTUX_SYSTEM_NAME win64)
  ELSE(WIN64)
    SET(SUPERTUX_SYSTEM_NAME win32)
  ENDIF(WIN64)

  SET(ENV{PATH} "$ENV{PATH};${DEPENDENCY_FOLDER}/include")
  SET(ENV{LIB} "${DEPENDENCY_FOLDER}/lib")
  SET(ENV{OPENALDIR} "${DEPENDENCY_FOLDER}")

  SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG DEBUG)
  ## Enable multi-processor compilation (faster)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  ## And shut up about unsafe stuff
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ## Add an icon
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/icon_rc.template" "${PROJECT_BINARY_DIR}/tmp/icon.rc")

  ## Find all used libraries
  FILE(GLOB DLLS "${DEPENDENCY_FOLDER}/dll/*")
ENDIF(WIN32)

## Add lots of dependencies to compiler switches

IF(WIN32)
  FIND_PATH(SDL2_INCLUDE_DIRS NAMES SDL.h PATHS "${DEPENDENCY_FOLDER}/include/SDL2")
  FIND_PATH(SDL2IMAGE_INCLUDE_DIRS NAMES SDL_image.h PATHS "${DEPENDENCY_FOLDER}/include/SDL2_image")
  FIND_LIBRARY(SDL2_LIBRARIES NAMES SDL2 PATHS "${DEPENDENCY_FOLDER}/lib")
  FIND_LIBRARY(SDL2IMAGE_LIBRARIES NAMES SDL2_image PATHS "${DEPENDENCY_FOLDER}/lib")
  FIND_LIBRARY(SDL2MAIN_LIBRARIES NAMES SDL2main PATHS "${DEPENDENCY_FOLDER}/lib")
ELSE(WIN32)
  INCLUDE(FindPkgConfig)
  ## 2.0.10 for SDL_RenderCopyExF that the software renderer's lighting
  ## needs to place a copy on a fraction of a pixel, and 2.0.18 for
  ## SDL_RenderSetVSync that the vsync setting changes through.
  PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2>=2.0.18)
  PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
ENDIF(WIN32)
SET(HAVE_SDL TRUE)

OPTION(BUILD_TESTS "Build test cases" OFF)
OPTION(BUILD_TESTS_M32 "Also run the squirrel samples at 32-bit integer width" OFF)
OPTION(ENABLE_OPENGL "Enable OpenGL support" ON)
## GLEW fetches the entry points a desktop driver keeps behind extensions.
## Hardware with a fixed 1.x pipeline has none to fetch and no GLEW built for
## it, so this has to come off without taking OpenGL with it.
OPTION(ENABLE_GLEW "Use GLEW to load OpenGL extensions" ON)
## Accepting a texture whose sides are not powers of two is an extension, so a
## build with a loader asks the driver at runtime. A build with no loader
## cannot ask and assumes the older rule, which rounds every texture up and
## roughly doubles the memory they occupy. Hardware known to take any size can
## say so here instead.
OPTION(ENABLE_NPOT_TEXTURES "Assume the hardware accepts non-power-of-two textures" OFF)
IF(ENABLE_NPOT_TEXTURES)
  SET(HAVE_NPOT_TEXTURES TRUE)
ENDIF()
## Hardware that stores textures in fixed size tiles wants each side rounded up
## to a multiple of the tile. 1 leaves the size alone.
SET(TEXTURE_ALIGNMENT 1 CACHE STRING "Round texture sides up to a multiple of this many pixels")
IF(NOT TEXTURE_ALIGNMENT MATCHES "^[1-9][0-9]*$")
  MESSAGE(FATAL_ERROR "TEXTURE_ALIGNMENT must be a positive whole number, got '${TEXTURE_ALIGNMENT}'")
ENDIF()
## The lightmap is drawn small and then multiplied over the finished frame.
## By default it is drawn in a corner of the screen and copied out afterwards,
## which needs glCopyTexSubImage2D. Where that call is missing but framebuffer
## objects are present, the lightmap can be drawn straight into its texture
## instead. Only one of the two is compiled in, since a target that lacks one
## call cannot compile the path that uses it.
OPTION(ENABLE_LIGHTMAP_FBO "Draw the lightmap into its texture with a framebuffer object" OFF)
OPTION(GLBINDING_ENABLED "Use glbinding instead of GLEW" OFF)
OPTION(GLBINDING_DEBUG_OUTPUT "Enable glbinding debug output for each called OpenGL function" OFF)
IF(ENABLE_OPENGL)
  FIND_PACKAGE(OpenGL)
  IF(OPENGL_FOUND)
    INCLUDE_DIRECTORIES(SYSTEM ${OPENGL_INCLUDE_DIR})
    SET(HAVE_OPENGL TRUE)
  ENDIF(OPENGL_FOUND)

  IF(GLBINDING_ENABLED)
    FIND_PACKAGE(GLBINDING REQUIRED)
    IF(GLBINDING_FOUND)
      INCLUDE_DIRECTORIES(SYSTEM ${GLBINDING_INCLUDES})
      ADD_DEFINITIONS(-DUSE_GLBINDING)
      IF (GLBINDING_DEBUG_OUTPUT)
        ADD_DEFINITIONS(-DUSE_GLBINDING_DEBUG_OUTPUT)
      ENDIF()
    ENDIF()
  ELSEIF(ENABLE_GLEW)
    FIND_PACKAGE(GLEW REQUIRED)
    IF(GLEW_FOUND)
      INCLUDE_DIRECTORIES(SYSTEM ${GLEW_INCLUDE_DIR})
      SET(HAVE_GLEW TRUE)
    ENDIF(GLEW_FOUND)
  ENDIF()
ENDIF(ENABLE_OPENGL)

## The interactive console is a debugging aid: it only opens in developer
## mode, and only from a key, so a target without a keyboard cannot reach it
## at all. Off by default, which saves some resources, though the buffer behind
## it is built either way and this is what carries log and script output to stderr.
OPTION(ENABLE_CONSOLE "Build the interactive console" OFF)

## Which sound backends to build in. Both can be built at once, and which
## one runs is then chosen with --audio-backend. The Wii has no usable
## OpenAL, so that one has to be switchable off.
OPTION(ENABLE_OPENAL "Build the OpenAL sound backend" ON)
OPTION(ENABLE_SDL_MIXER "Build the SDL_mixer sound backend" OFF)

IF(NOT ENABLE_OPENAL AND NOT ENABLE_SDL_MIXER)
  MESSAGE(FATAL_ERROR "Enable at least one of ENABLE_OPENAL and ENABLE_SDL_MIXER")
ENDIF()

IF(ENABLE_OPENAL)
  FIND_PACKAGE(OpenAL REQUIRED)
  INCLUDE_DIRECTORIES(SYSTEM ${OPENAL_INCLUDE_DIR})
ENDIF(ENABLE_OPENAL)

IF(ENABLE_SDL_MIXER)
  IF(WIN32)
    FIND_PATH(SDL2MIXER_INCLUDE_DIRS NAMES SDL_mixer.h PATHS "${DEPENDENCY_FOLDER}/include/SDL2_mixer")
    FIND_LIBRARY(SDL2MIXER_LIBRARIES NAMES SDL2_mixer PATHS "${DEPENDENCY_FOLDER}/lib")
  ELSE(WIN32)
    PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
  ENDIF(WIN32)
ENDIF(ENABLE_SDL_MIXER)

FIND_PACKAGE(OggVorbis REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${VORBIS_INCLUDE_DIR})

## The version. The first three numbers are the upstream release this fork is
## built from; the suffix is this fork's own revision, bumped by hand. The same
## arrangement SuperTux-Wii uses, and it needs nothing from git, so a build from
## a downloaded source archive reports the same version as a checkout.
SET(SUPERTUX_VERSION_MAJOR 0)
SET(SUPERTUX_VERSION_MINOR 5)
SET(SUPERTUX_VERSION_PATCH 1)
SET(SUPERTUX_VERSION_TWEAK )
SET(SUPERTUX_WII_REVISION "d.00")
SET(SUPERTUX_VERSION_STRING "${SUPERTUX_VERSION_MAJOR}.${SUPERTUX_VERSION_MINOR}.${SUPERTUX_VERSION_PATCH}-wii-${SUPERTUX_WII_REVISION}")

## A build that is not the release one says so in the version.
IF(NOT CMAKE_BUILD_TYPE MATCHES Release)
  SET(SUPERTUX_VERSION_STRING "${SUPERTUX_VERSION_STRING}-dev")
ENDIF()


SET(SUPERTUX_VERSION ${SUPERTUX_VERSION_STRING})

## Also build simplesquirrel and the squirrel it binds

## Built here rather than through their own CMake, which exists to install a
## shared library and a pkg-config file we do not ship. An ADD_SUBDIRECTORY at
## this point in the file also misses the CMAKE_CXX_FLAGS set further down.
SET(SQUIRREL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/squirrel)
SET(SIMPLESQUIRREL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/simplesquirrel)

FILE(GLOB SQUIRREL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SQUIRREL_DIR}/squirrel/*.cpp)
FILE(GLOB SQSTDLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SQUIRREL_DIR}/sqstdlib/*.cpp)
FILE(GLOB SIMPLESQUIRREL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SIMPLESQUIRREL_DIR}/source/*.cpp)

ADD_LIBRARY(squirrel STATIC ${SQUIRREL_SOURCES})
ADD_LIBRARY(sqstdlib STATIC ${SQSTDLIB_SOURCES})
ADD_LIBRARY(simplesquirrel STATIC ${SIMPLESQUIRREL_SOURCES})

## These headers compile inside our own translation units, so they have to be
## system headers or our warning flags are held against their code.
TARGET_INCLUDE_DIRECTORIES(squirrel SYSTEM PUBLIC ${SQUIRREL_DIR}/include)
TARGET_INCLUDE_DIRECTORIES(simplesquirrel SYSTEM PUBLIC ${SIMPLESQUIRREL_DIR}/include)
TARGET_LINK_LIBRARIES(sqstdlib PUBLIC squirrel)
TARGET_LINK_LIBRARIES(simplesquirrel PUBLIC squirrel sqstdlib)

## _SQ64 widens SQInteger, so it has to reach every translation unit that sees
## squirrel.h and not just the library's own.
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  TARGET_COMPILE_DEFINITIONS(squirrel PUBLIC _SQ64)
ENDIF()

IF(WIN32)
	#For debug run purposes
	CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/run_supertux.bat.in" "${PROJECT_BINARY_DIR}/run_supertux.bat")
ENDIF()

## Some additional include paths

include_directories(${CMAKE_BINARY_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/obstack/)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/include/)

IF(WIN32)
  ADD_DEFINITIONS(-DNOMINMAX)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

## Build list of sources for supertux binary

FILE(GLOB SUPERTUX_SOURCES_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} external/obstack/*.c)

## This glob reaches src/lib/*.cpp as well, so nothing vendored may sit at that
## depth or it lands in our build under our warning flags.
FILE(GLOB SUPERTUX_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.cpp src/supertux/menu/*.cpp src/video/sdl/*.cpp)
IF(NOT ENABLE_SDL_MIXER)
  LIST(REMOVE_ITEM SUPERTUX_SOURCES_CXX src/audio/sdl_mixer_device.cpp src/audio/sdl_sound_source.cpp)
ENDIF(NOT ENABLE_SDL_MIXER)
IF(NOT ENABLE_OPENAL)
  LIST(REMOVE_ITEM SUPERTUX_SOURCES_CXX
       src/audio/openal_device.cpp src/audio/openal_sound_source.cpp
       src/audio/stream_sound_source.cpp)
ENDIF(NOT ENABLE_OPENAL)
FILE(GLOB SUPERTUX_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROJECT_BINARY_DIR}/tmp/*.rc")

IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/CMakeLists.txt)
  message(FATAL_ERROR "sexp-cpp submodule is not checked out or ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/CMakeLists.txt is missing")
ENDIF()
FILE(GLOB SEXP_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} external/sexp-cpp/src/*.cpp)
ADD_LIBRARY(sexp STATIC ${SEXP_SOURCES_CXX})


IF(HAVE_OPENGL)
  FILE(GLOB SUPERTUX_OPENGL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/video/gl/*.cpp)
  SET(SUPERTUX_SOURCES_CXX ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_OPENGL_SOURCES})
ENDIF(HAVE_OPENGL)

## Sort source lists to have deterministic linking order
LIST(SORT SUPERTUX_SOURCES_C)
LIST(SORT SUPERTUX_SOURCES_CXX)
LIST(SORT SUPERTUX_RESOURCES)

## Compile everything at once (roughly equivalent to cat *.cpp | gcc)

OPTION(COMPILE_AMALGATION "Compile all the files together at once (experimental)" OFF)
IF(COMPILE_AMALGATION)
  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp "// Includes all source files of the project\n")
  FOREACH(ST_FILE ${SUPERTUX_SOURCES_CXX})
    FILE(RELATIVE_PATH CXX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src "${CMAKE_CURRENT_SOURCE_DIR}/${ST_FILE}")
    FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp "#include \"${CXX_FILE}\"\n")
  ENDFOREACH(ST_FILE)
  ## Build instead of CXX sources
  SET(SUPERTUX_SOURCES_CXX ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
ELSE(COMPILE_AMALGATION)
  IF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
    FILE(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
  ENDIF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
ENDIF(COMPILE_AMALGATION)

## Debug options

OPTION(WERROR "Stop on first compiler warning" OFF)
OPTION(WARNINGS "Enable long list of warnings for compiler to check" OFF)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  SET(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  SET(CMAKE_CXX_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  SET(CMAKE_C_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  SET(CMAKE_LD_FLAGS_PROFILE "-lgmon" CACHE STRING "Profile flags")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funit-at-a-time")

  ## Warnings go on our own targets rather than into CMAKE_CXX_FLAGS, so that
  ## vendored code under external/ is judged by its own build and not by ours.
  SET(SUPERTUX2_WARNING_FLAGS -Wall -Wextra -Wno-unused-parameter)
  IF(WERROR)
    LIST(APPEND SUPERTUX2_WARNING_FLAGS -Werror)
  ENDIF(WERROR)
  IF(WARNINGS)
    # temporarily disabled:
    #   -Wfloat-equal -Wconversion -Wundef -Wswitch-default
    #   -Wswitch-enum -Wcast-qual -Woverloaded-virtual -Wmissing-format-attribute
    #   -Wold-style-cast -Wpadded -Wabi -Winline -Wunsafe-loop-optimizations -Wstrict-overflow=5
    # fails on MinGW:
    #   -ansi
    # fails on clang:
    #   -Wlogical-op -Wstrict-null-sentinel
    LIST(APPEND SUPERTUX2_WARNING_FLAGS
      -fdiagnostics-show-option -pedantic -Wno-long-long -Wcast-align
      -Wdisabled-optimization -Winit-self -Winvalid-pch -Wmissing-include-dirs
      -Wmissing-noreturn -Wpacked -Wredundant-decls -Wstack-protector
      -Wformat=2 -Weffc++ -Wctor-dtor-privacy -Wshadow -Wnon-virtual-dtor
      -Wcast-qual -Wsign-promo -Wsign-conversion)
  ENDIF(WARNINGS)
endif()

## Some additional compiler switches
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")

  ## A section per function and per object lets the linker drop what nothing reaches.
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

## On Windows, add an icon

IF(WIN32)
#  SET(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/supertux.rc)
  IF(MINGW)
    ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o
    COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows -i${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/supertux.rc -o ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
    SET(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
  ENDIF(MINGW)
ENDIF(WIN32)

## Generate supertux executable in the right place

#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

## Add target for supertux binary

add_library(supertux2_c OBJECT ${SUPERTUX_SOURCES_C})
add_library(supertux2_lib STATIC ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_RESOURCES} $<TARGET_OBJECTS:supertux2_c>)
IF(WIN32)
    add_executable(supertux2 WIN32 src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/supertux.rc)
    TARGET_LINK_LIBRARIES(supertux2 ${SDL2MAIN_LIBRARIES})
ELSE()
    add_executable(supertux2 src/main.cpp)
ENDIF(WIN32)
target_link_libraries(supertux2 supertux2_lib)
set_target_properties(supertux2 PROPERTIES OUTPUT_NAME supertux2-wii)
set_target_properties(supertux2_lib PROPERTIES OUTPUT_NAME supertux2)
target_compile_options(supertux2_lib PRIVATE ${SUPERTUX2_WARNING_FLAGS})
target_compile_options(supertux2 PRIVATE ${SUPERTUX2_WARNING_FLAGS})

## These trees take our flags, less the exceptions below. -Weffc++ wants members
## named in constructor initializer lists where squirrel sets them in the body,
## but that's all the flag can see. -Wsign-conversion lands on SQInteger going to
## an unsigned type. Squirrel returns -1 as a sentinel, so a plain cast would
## hide a real value.
TARGET_COMPILE_OPTIONS(squirrel PRIVATE ${SUPERTUX2_WARNING_FLAGS}
  -Wno-effc++ -Wno-sign-conversion)
TARGET_COMPILE_OPTIONS(sqstdlib PRIVATE ${SUPERTUX2_WARNING_FLAGS})
TARGET_COMPILE_OPTIONS(simplesquirrel PRIVATE ${SUPERTUX2_WARNING_FLAGS})

IF(WIN32)
  ## Copy dlls on windows
  ADD_CUSTOM_COMMAND(TARGET supertux2_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${DEPENDENCY_FOLDER}/dll"
      $<TARGET_FILE_DIR:supertux2_lib>)
#  ADD_CUSTOM_COMMAND(TARGET supertux2 POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#     "${DEPENDENCY_FOLDER}/DigiCertHighAssuranceEVRootCA.pem"
#     $<TARGET_FILE_DIR:supertux2_lib>)
ENDIF(WIN32)

## Link supertux binary with squirrel and other libraries

TARGET_INCLUDE_DIRECTORIES(supertux2_lib SYSTEM PUBLIC ${SDL2_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(supertux2_lib SYSTEM PUBLIC ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${SDL2_LIBRARIES})
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${SDL2IMAGE_LIBRARIES})

TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC simplesquirrel)
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC sexp)
IF(ENABLE_OPENAL)
  TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OPENAL_LIBRARY})
ENDIF(ENABLE_OPENAL)
IF(ENABLE_SDL_MIXER)
  TARGET_INCLUDE_DIRECTORIES(supertux2_lib SYSTEM PUBLIC ${SDL2MIXER_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${SDL2MIXER_LIBRARIES})
ENDIF(ENABLE_SDL_MIXER)
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OGGVORBIS_LIBRARIES})

IF(HAVE_OPENGL)
    TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OPENGL_LIBRARY})
    IF(GLBINDING_FOUND)
      TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${GLBINDING_LIBRARIES})
    ELSEIF(HAVE_GLEW)
      TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${GLEW_LIBRARY})
    ENDIF()
ENDIF(HAVE_OPENGL)

if(BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()

  # each test file is its own program, so a failure names the file that failed
  FUNCTION(MAKE_UNIT_TEST TEST_NAME SOURCE)
    add_executable(${TEST_NAME} ${SOURCE})
    target_compile_options(${TEST_NAME} PRIVATE ${SUPERTUX2_WARNING_FLAGS})
    target_link_libraries(${TEST_NAME} supertux2_lib)
    add_test(NAME ${TEST_NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${TEST_NAME})
  ENDFUNCTION()

  MAKE_UNIT_TEST(aatriangle_test tests/aatriangle_test.cpp)
  MAKE_UNIT_TEST(reader_test tests/reader_test.cpp)
  MAKE_UNIT_TEST(size_test tests/size_test.cpp)
  MAKE_UNIT_TEST(sizef_test tests/sizef_test.cpp)
  MAKE_UNIT_TEST(string_util_test tests/string_util_test.cpp)

  ## sq reaches squirrel and sqstdlib but none of simplesquirrel's own code.
  ADD_EXECUTABLE(simplesquirrel_test tests/simplesquirrel_test.cpp)
  TARGET_COMPILE_OPTIONS(simplesquirrel_test PRIVATE ${SUPERTUX2_WARNING_FLAGS})
  TARGET_INCLUDE_DIRECTORIES(simplesquirrel_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  TARGET_LINK_LIBRARIES(simplesquirrel_test simplesquirrel)
  ADD_TEST(NAME simplesquirrel_test COMMAND simplesquirrel_test)

  ## Squirrel's own interpreter, built to run squirrel's own samples against
  ## output recorded from a tree we have not edited. This is what says the VM
  ## still behaves after we touch it, since nothing else we have exercises the
  ## compiler and the VM directly.
  ADD_EXECUTABLE(sq tests/squirrel/sq.c)
  TARGET_LINK_LIBRARIES(sq squirrel sqstdlib)
  SET_TARGET_PROPERTIES(sq PROPERTIES C_STANDARD 99)

  ## Six of the samples are Computer Language Shootout benchmarks that take
  ## their problem size on the command line and fall back to 1, which computes
  ## nothing worth recording. These sizes run in well under a second each.
  SET(SQ_ARGS_ackermann 7)
  SET(SQ_ARGS_array 2000)
  SET(SQ_ARGS_fibonacci 25)
  SET(SQ_ARGS_list 3)
  SET(SQ_ARGS_matrix 40)
  SET(SQ_ARGS_methcall 200000)

  ## _SQ64 is set only on 64-bit, so a 32-bit build gets int where this one gets
  ## long long, and every sample recording was made at the wider of the two.
  IF(BUILD_TESTS_M32)
    INCLUDE(CheckCXXSourceCompiles)
    SET(CMAKE_REQUIRED_FLAGS "-m32")
    CHECK_CXX_SOURCE_COMPILES("int main(){return 0;}" HAVE_M32)
    UNSET(CMAKE_REQUIRED_FLAGS)
    IF(NOT HAVE_M32)
      MESSAGE(FATAL_ERROR "BUILD_TESTS_M32 needs a compiler that can target -m32")
    ENDIF()

    ## Built from sources rather than the squirrel targets, which carry _SQ64
    ## and are the wrong width to link against.
    ADD_EXECUTABLE(sq32 tests/squirrel/sq.c ${SQUIRREL_SOURCES} ${SQSTDLIB_SOURCES})
    TARGET_INCLUDE_DIRECTORIES(sq32 SYSTEM PRIVATE ${SQUIRREL_DIR}/include)
    TARGET_INCLUDE_DIRECTORIES(sq32 PRIVATE ${SQUIRREL_DIR}/squirrel ${SQUIRREL_DIR}/sqstdlib)
    ## No warning flags, as with sq: this target exists to run the samples, and
    ## sq.c compiles as C where most of our list is C++ only.
    TARGET_COMPILE_OPTIONS(sq32 PRIVATE -m32)
    TARGET_LINK_OPTIONS(sq32 PRIVATE -m32)
    SET_TARGET_PROPERTIES(sq32 PROPERTIES C_STANDARD 99)
  ENDIF()

  FILE(GLOB SQUIRREL_SAMPLES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/samples
       ${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/samples/*.nut)
  FOREACH(SAMPLE ${SQUIRREL_SAMPLES})
    GET_FILENAME_COMPONENT(SAMPLE_NAME ${SAMPLE} NAME_WE)
    SET(SQ_TARGETS sq)
    IF(BUILD_TESTS_M32)
      LIST(APPEND SQ_TARGETS sq32)
    ENDIF()
    FOREACH(SQ_TARGET ${SQ_TARGETS})
      IF(SQ_TARGET STREQUAL sq)
        SET(CASE squirrel_${SAMPLE_NAME})
      ELSE()
        SET(CASE squirrel_m32_${SAMPLE_NAME})
      ENDIF()
      ADD_TEST(NAME ${CASE}
        COMMAND ${CMAKE_COMMAND}
          -DSQ=$<TARGET_FILE:${SQ_TARGET}>
          -DSCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/samples/${SAMPLE}
          -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/expected/${SAMPLE_NAME}.txt
          -DARGS=${SQ_ARGS_${SAMPLE_NAME}}
          -DTEST_NAME=${CASE}
          -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/run_sample.cmake)
      ## These finish in well under a second, so the cap only fires on a stuck VM.
      SET_TESTS_PROPERTIES(${CASE} PROPERTIES TIMEOUT 30)
    ENDFOREACH()
  ENDFOREACH()
endif()

## Install stuff

## Sets CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS that the VCREDIST branch and CPack both read.
INCLUDE(InstallRequiredSystemLibraries)

IF(WIN32 AND NOT UNIX)

  SET(INSTALL_SUBDIR_BIN "bin" CACHE STRING "Installation subdir for binaries")
  SET(INSTALL_SUBDIR_SHARE "data" CACHE STRING "Installation subdir for data")
  SET(INSTALL_SUBDIR_DOC "doc" CACHE STRING "Installation subdir for docs")

  INSTALL(FILES ${DLLS} DESTINATION ${INSTALL_SUBDIR_BIN})

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/packaging/supertux2-wii.png ${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/supertux.ico DESTINATION ".")

  OPTION(PACKAGE_VCREDIST "Package the VCREDIST libraries with the program" OFF)

  IF(PACKAGE_VCREDIST)
    INSTALL(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${INSTALL_SUBDIR_BIN})
    include(FindWindowsSDK)
    IF(${CMAKE_VS_PLATFORM_NAME} STREQUAL "Win32")
        SET(UCRT_PATH "${WINDOWSSDK_PREFERRED_DIR}/Redist/ucrt/DLLs/x86")
    ELSE()
        SET(UCRT_PATH "${WINDOWSSDK_PREFERRED_DIR}/Redist/ucrt/DLLs/x64")
    ENDIF()
    FILE(GLOB UCRT_DLLS "${UCRT_PATH}/*")
    INSTALL(FILES ${UCRT_DLLS} DESTINATION ${INSTALL_SUBDIR_BIN})
  ENDIF()

ELSE(WIN32 AND NOT UNIX)
  SET(INSTALL_SUBDIR_BIN "games" CACHE STRING "Installation subdir for binaries")
  SET(INSTALL_SUBDIR_SHARE "share/games/supertux2-wii" CACHE STRING "Installation subdir for data")
  SET(INSTALL_SUBDIR_DOC "share/doc/supertux2-wii" CACHE STRING "Installation subdir for docs")

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/packaging/linux/supertux2-wii.desktop DESTINATION "share/applications")

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/packaging/supertux2-wii.png ${CMAKE_CURRENT_SOURCE_DIR}/packaging/linux/supertux2-wii.xpm DESTINATION "share/pixmaps/")

ENDIF(WIN32 AND NOT UNIX)

INSTALL(TARGETS supertux2 DESTINATION ${INSTALL_SUBDIR_BIN})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/INSTALL.md ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${INSTALL_SUBDIR_DOC})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/credits.stxt DESTINATION ${INSTALL_SUBDIR_SHARE})


INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/images
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/music
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/scripts
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/speech
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/sounds
                  DESTINATION ${INSTALL_SUBDIR_SHARE})

IF(CMAKE_BUILD_TYPE MATCHES Release)
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
                    DESTINATION ${INSTALL_SUBDIR_SHARE}
                    PATTERN "data/levels/incubator" EXCLUDE)
ELSE()
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
                    DESTINATION ${INSTALL_SUBDIR_SHARE})
ENDIF()

## Create config.h now that INSTALL_SUBDIR_* have been set.

configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h )


## CPack/Installation-specific stuff

SET(CPACK_PACKAGE_NAME "SuperTux2-Wii")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Jump'n'Run Game featuring Tux")
SET(CPACK_PACKAGE_VENDOR "SuperTux Devel Team")
SET(CPACK_PACKAGE_CONTACT "SuperTux Devel Team <supertux-devel@lists.lethargik.org>")
SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;${CMAKE_BINARY_DIR};/\\\\..*")
SET(CPACK_DEBIAN_PACKAGE_NAME "supertux2-wii")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.5), libgcc1 (>= 1:4.1), libgl1-mesa-glx | libgl1, libogg0 (>= 1.1.3), libopenal0a, libsdl-image1.2 (>= 1.2.5), libsdl1.2debian (>= 1.2.10-1), libstdc++6 (>= 4.1.2), libvorbis0a (>= 1.1.2), libvorbisfile3 (>= 1.1.2)")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Classic 2D jump 'n run sidescroller with Tux\n SuperTux is a classic 2D jump 'n run sidescroller game in a similar\n style like the original SuperMario games. This release of SuperTux\n features 9 enemies, 26 playable levels, software and OpenGL rendering\n modes, configurable joystick and keyboard input, new music and\n completely redone graphics.\n .\n This is a development snapshot of SuperTux. It may suffer from\n critical bugs and has not been fully tested. \n .\n Homepage: http://supertux.lethargik.org/")
SET(CPACK_DEBIAN_PACKAGE_SECTION "games")
SET(CPACK_RPM_PACKAGE_NAME "supertux2-wii")
SET(CPACK_RPM_PACKAGE_DESCRIPTION "Classic 2D jump 'n run sidescroller with Tux\n SuperTux is a classic 2D jump 'n run sidescroller game in a similar\n style like the original SuperMario games. This release of SuperTux\n features 9 enemies, 26 playable levels, software and OpenGL rendering\n modes, configurable joystick and keyboard input, new music and\n completely redone graphics.\n .\n This is a development snapshot of SuperTux. It may suffer from\n critical bugs and has not been fully tested. \n .\n Homepage: http://supertux.lethargik.org/")
SET(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
SET(CPACK_RPM_PACKAGE_GROUP "Amusements/Games/Action/Arcade")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR ${SUPERTUX_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${SUPERTUX_VERSION_MINOR})

IF(SUPERTUX_VERSION_TWEAK)
    SET(CPACK_PACKAGE_VERSION_PATCH "${SUPERTUX_VERSION_PATCH}.${SUPERTUX_VERSION_TWEAK}")
ELSE()
    SET(CPACK_PACKAGE_VERSION_PATCH ${SUPERTUX_VERSION_PATCH})
ENDIF(SUPERTUX_VERSION_TWEAK)
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SUPERTUX_VERSION_STRING}-${SUPERTUX_SYSTEM_NAME}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SUPERTUX_VERSION_STRING}-Source")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "SuperTux")
SET(CPACK_PACKAGE_EXECUTABLES "supertux2-wii" "SuperTux 2 Wii")
IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "supertux2-wii")
  SET(CPACK_NSIS_DISPLAY_NAME "SuperTux")
  SET(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  SET(CPACK_WIX_PROGRAM_MENU_FOLDER "")
  SET(CPACK_WIX_UPGRADE_GUID "93E16F4E-0A68-422A-8ADC-47BE5B9433B2")
  SET(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/supertux.ico")

  SET(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/mk/wix/banner.png")
  SET(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/mk/wix/dialog.png")
  SET(CPACK_GENERATOR WIX)
ELSE(WIN32 AND NOT UNIX)
ENDIF(WIN32 AND NOT UNIX)
INCLUDE(CPack)

# move some config clutter to the advanced section
MARK_AS_ADVANCED(
  INSTALL_SUBDIR_BIN
  INSTALL_SUBDIR_SHARE
  INSTALL_SUBDIR_DOC
)

MARK_AS_ADVANCED(
  CMAKE_BACKWARDS_COMPATIBILITY
  CMAKE_BUILD_TYPE
  CMAKE_INSTALL_PREFIX
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
)

MARK_AS_ADVANCED(
  GLEW_INCLUDE_DIR
  GLEW_LIBRARY
)

MARK_AS_ADVANCED(
  SDL2_INCLUDE_DIRS
  SDL2_LIBRARIES
)

MARK_AS_ADVANCED(
  SDL2IMAGE_INCLUDE_DIRS
  SDL2IMAGE_LIBRARIES
)

MARK_AS_ADVANCED(
  OPENAL_INCLUDE_DIR
  OPENAL_LIBRARY
)

MARK_AS_ADVANCED(
  OGG_LIBRARY
  VORBISENC_LIBRARY
  VORBISFILE_LIBRARY
  VORBIS_INCLUDE_DIR
  VORBIS_LIBRARY
)

