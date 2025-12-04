# CMakeLists.txt
#
# SuperTux - root build script
# Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
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


# Allow building with CMake 4.0. Set compatibility options. Use some bundled modules as a fallback
CMAKE_MINIMUM_REQUIRED(VERSION 4.0)

## Project name to use as command prefix.
PROJECT(SUPERTUX)

## Detect Wii platform (Must be BEFORE endian check)
IF(CMAKE_TOOLCHAIN_FILE MATCHES "Wii.cmake")
  SET(WII TRUE)
  MESSAGE(STATUS "Building for Nintendo Wii")

  # Wii requires these settings
  SET(ENABLE_OPENGL ON CACHE BOOL "Enable OpenGL support" FORCE)
  SET(ENABLE_OPENAL OFF CACHE BOOL "Disable OpenAL (use SDL2_mixer)" FORCE)
  SET(BUILD_TESTS OFF CACHE BOOL "Disable tests on Wii" FORCE)

  # Force POT textures on Wii (no NPOT support in OpenGX)
  SET(ENABLE_NPOT_TEXTURES OFF CACHE BOOL "Wii requires POT textures" FORCE)

  # Force round-trip lightmap on Wii (glCopyTexSubImage2D not available in OpenGX)
  SET(FORCE_ROUNDTRIP_LIGHTMAP ON CACHE BOOL "Wii requires round-trip lightmap" FORCE)

  # Wii-specific definitions
  ADD_DEFINITIONS(-D_WII_ -DGEKKO -DSDL_MAIN_HANDLED -DDISABLE_CONSOLE)

  # Set installation paths for Wii (sd:/apps/supertux2/)
  SET(INSTALL_SUBDIR_BIN "" CACHE STRING "Installation subdir for binaries")
  SET(INSTALL_SUBDIR_SHARE "data" CACHE STRING "Installation subdir for data")
  SET(INSTALL_SUBDIR_DOC "docs" CACHE STRING "Installation subdir for docs")
ENDIF()

### CMake configuration

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake)
INCLUDE(ConfigureFiles)
INCLUDE(ExternalProject)

## For autopackage
SET(APPDATADIR "${CMAKE_INSTALL_PREFIX}/share/games/supertux2")

SET(BUILD_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
SET(BUILD_CONFIG_DATA_DIR "${CMAKE_BINARY_DIR}/data")
## Check endianess
IF(WII)
  # PowerPC is always big-endian, skip the test for cross-compilation
  SET(WORDS_BIGENDIAN 1)
  MESSAGE(STATUS "Wii/PowerPC: Big-endian architecture")
ELSE()
  INCLUDE(TestBigEndian)
  TEST_BIG_ENDIAN(WORDS_BIGENDIAN)
ENDIF()

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
ELSEIF(WII)
  # Wii uses portlibs
  SET(SDL2_INCLUDE_DIRS "${PORTLIBS}/include/SDL2")
  SET(SDL2IMAGE_INCLUDE_DIRS "${PORTLIBS}/include/SDL2")
  # Libraries are linked explicitly at the end to ensure correct order
  SET(SDL2_LIBRARIES "")  # Will be added to final link step
  SET(SDL2IMAGE_LIBRARIES "")
  MESSAGE(STATUS "Wii SDL2 configured from portlibs")
ELSE(WIN32)
  INCLUDE(FindPkgConfig)
  PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2>=2.0.1)
  PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
ENDIF(WIN32)
SET(HAVE_SDL TRUE)

OPTION(BUILD_TESTS "Build test cases" OFF)
OPTION(ENABLE_OPENGL "Enable OpenGL support" ON)
OPTION(GLBINDING_ENABLED "Use glbinding instead of GLEW" OFF)
OPTION(GLBINDING_DEBUG_OUTPUT "Enable glbinding debug output for each called OpenGL function" OFF)

IF(ENABLE_OPENGL)
  IF(WII)
    # Wii uses OpenGX (OpenGL 1.2 wrapper for GX)
    MESSAGE(STATUS "Using OpenGX for Wii OpenGL support")
    SET(OPENGL_FOUND TRUE)
    SET(OPENGL_LIBRARIES "-lGLU -lglut -lopengx")
    SET(OPENGL_INCLUDE_DIR "${PORTLIBS}/include/GL")
    INCLUDE_DIRECTORIES(SYSTEM ${OPENGL_INCLUDE_DIR})
    SET(HAVE_OPENGL TRUE)
    SET(HAVE_GLEW FALSE) # Explicitly disable GLEW for Wii
  ELSE()
    FIND_PACKAGE(OpenGL)
    IF(OPENGL_FOUND)
      INCLUDE_DIRECTORIES(SYSTEM ${OPENGL_INCLUDE_DIR})
      SET(HAVE_OPENGL TRUE)
    ENDIF(OPENGL_FOUND)

    SET(HAVE_GLEW FALSE)

  ENDIF()

  IF(GLBINDING_ENABLED)
    FIND_PACKAGE(GLBINDING REQUIRED)
    IF(GLBINDING_FOUND)
      INCLUDE_DIRECTORIES(SYSTEM ${GLBINDING_INCLUDES})
      ADD_DEFINITIONS(-DUSE_GLBINDING)
      IF (GLBINDING_DEBUG_OUTPUT)
        ADD_DEFINITIONS(-DUSE_GLBINDING_DEBUG_OUTPUT)
      ENDIF()
    ENDIF()
  ELSE()
    # Only require GLEW on desktop platforms
    IF(NOT GL_VERSION_ES_CM_1_0)
      FIND_PACKAGE(GLEW REQUIRED)
      IF(GLEW_FOUND)
        INCLUDE_DIRECTORIES(SYSTEM ${GLEW_INCLUDE_DIR})
        SET(HAVE_GLEW TRUE)
      ENDIF(GLEW_FOUND)
    ENDIF()
  ENDIF()

  # NPOT (Non-Power-Of-Two) texture support detection
  # Desktop OpenGL with GLEW typically supports this via ARB extension
  # OpenGL ES and embedded platforms may not support it
  OPTION(ENABLE_NPOT_TEXTURES "Enable non-power-of-two texture support (requires hardware support)" ON)

  IF(ENABLE_NPOT_TEXTURES)
    # On desktop with GLEW, NPOT is detected at runtime via GLEW_ARB_texture_non_power_of_two
    # On other platforms, we assume no NPOT support unless explicitly enabled
    IF(NOT GL_VERSION_ES_CM_1_0 AND NOT GLBINDING_ENABLED)
      # Desktop with GLEW - runtime detection will work
      SET(HAVE_NPOT_TEXTURES TRUE)
    ELSEIF(GLBINDING_ENABLED)
      # glbinding - runtime detection via ContextInfo
      SET(HAVE_NPOT_TEXTURES TRUE)
    ELSE()
      # OpenGL ES or other - assume no NPOT unless platform explicitly enables it
      SET(HAVE_NPOT_TEXTURES FALSE)
    ENDIF()
  ELSE()
    SET(HAVE_NPOT_TEXTURES FALSE)
  ENDIF()

  IF(HAVE_NPOT_TEXTURES)
    MESSAGE(STATUS "NPOT texture support: Enabled (runtime detection)")
  ELSE()
    MESSAGE(STATUS "NPOT texture support: Disabled (will use power-of-two textures)")
  ENDIF()

  # Lightmap rendering support
  # Lightmaps provide dynamic lighting effects but require framebuffer operations
  # that may not be available or performant on all platforms
  OPTION(ENABLE_LIGHTMAP "Enable lightmap rendering (requires FBO or glReadPixels support)" ON)

  IF(ENABLE_LIGHTMAP)
    SET(HAVE_LIGHTMAP TRUE)
    MESSAGE(STATUS "Lightmap rendering: Enabled")

    # Detect if we need to use round-trip mode (glReadPixels + glTexSubImage2D)
    # instead of native mode (glCopyTexSubImage2D)
    # This is needed on platforms where glCopyTexSubImage2D is not implemented
    OPTION(FORCE_ROUNDTRIP_LIGHTMAP "Force round-trip lightmap mode (for testing)" OFF)

    IF(FORCE_ROUNDTRIP_LIGHTMAP)
      SET(USE_ROUNDTRIP_LIGHTMAP TRUE)
      MESSAGE(STATUS "Lightmap mode: ROUNDTRIP (forced)")
    ELSE()
      # Desktop platforms use native mode by default
      # This will be overridden for specific platforms (like Wii) later
      SET(USE_ROUNDTRIP_LIGHTMAP FALSE)
      MESSAGE(STATUS "Lightmap mode: NATIVE (auto-detected)")
    ENDIF()
  ELSE()
    MESSAGE(STATUS "Lightmap rendering: Disabled")
  ENDIF()

ENDIF(ENABLE_OPENGL)

OPTION(ENABLE_OPENAL "Enable OpenAL support (Disable for SDL2_mixer backend)" ON)

IF(ENABLE_OPENAL)
  FIND_PACKAGE(OpenAL REQUIRED)
  INCLUDE_DIRECTORIES(SYSTEM ${OPENAL_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAVE_OPENAL)

  FIND_PACKAGE(OggVorbis REQUIRED)
  INCLUDE_DIRECTORIES(SYSTEM ${VORBIS_INCLUDE_DIR})
ELSE()
  # SDL2_mixer Logic
  IF(WIN32)
    FIND_PATH(SDL2MIXER_INCLUDE_DIRS NAMES SDL_mixer.h PATHS "${DEPENDENCY_FOLDER}/include/SDL2")
    FIND_LIBRARY(SDL2MIXER_LIBRARIES NAMES SDL2_mixer PATHS "${DEPENDENCY_FOLDER}/lib")
  ELSEIF(WII)
    SET(SDL2MIXER_INCLUDE_DIRS "${PORTLIBS}/include/SDL2")
    # Libraries are linked explicitly at the end
    MESSAGE(STATUS "Wii SDL2_mixer configured")
  ELSE()
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
  ENDIF()

  INCLUDE_DIRECTORIES(SYSTEM ${SDL2MIXER_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DUSE_SDL_MIXER)
ENDIF()

INCLUDE(CheckSymbolExists)

#FIND_PACKAGE(ICONV REQUIRED)
#INCLUDE_DIRECTORIES(SYSTEM ${ICONV_INCLUDE_DIR})
#LINK_LIBRARIES(${ICONV_LIBRARY})

IF(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  ## Find revision of WC
  MARK_AS_ADVANCED(GIT_EXECUTABLE)
  FIND_PROGRAM(GIT_EXECUTABLE git)
  IF(NOT GIT_EXECUTABLE EQUAL "GIT_EXECUTABLE-NOTFOUND")
    INCLUDE(GetGitRevisionDescription)
    git_describe(VERSION_STRING_GIT "--tags" "--match" "?[0-9]*.[0-9]*.[0-9]*")
    STRING(REPLACE "v" "" VERSION_LIST ${VERSION_STRING_GIT})
    STRING(REGEX REPLACE "(-|_|\\.)" ";" VERSION_LIST ";${VERSION_LIST}")
  ENDIF()
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/.git")

GET_FILENAME_COMPONENT(BASEDIR ${CMAKE_SOURCE_DIR} NAME)
IF("${VERSION_LIST}" STREQUAL "")
  IF(${BASEDIR} MATCHES "supertux2-[0-9\\.]*")
    STRING(REGEX REPLACE "(\\.|_|-)" ";" VERSION_LIST ${BASEDIR})
  ENDIF()
ENDIF()

FILE(GLOB ORIG_TGZ ../*.orig.tar.gz)
IF("${VERSION_LIST}" STREQUAL "" AND (NOT "${ORIG_TGZ}" STREQUAL ""))
  GET_FILENAME_COMPONENT(BASEDIR ${ORIG_TGZ} NAME)
  STRING(REGEX REPLACE "(\\.|_|-)" ";" VERSION_LIST ${BASEDIR})
ENDIF()

LIST(LENGTH VERSION_LIST VERSION_LIST_SIZE)

IF(${VERSION_LIST_SIZE} GREATER 0)
  LIST(GET VERSION_LIST 1 MAJOR_VERSION_GIT)
  LIST(GET VERSION_LIST 2 MINOR_VERSION_GIT)
  LIST(GET VERSION_LIST 3 PATCH_VERSION_GIT)

  IF("${VERSION_STRING_GIT}" STREQUAL "")
    SET(VERSION_STRING_GIT "${MAJOR_VERSION_GIT}.${MINOR_VERSION_GIT}.${PATCH_VERSION_GIT}")
  ENDIF()

  CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/version.cmake.in" "${CMAKE_SOURCE_DIR}/version.cmake")
ENDIF()
IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/version.cmake")
  MESSAGE( SEND_ERROR "Cound not find GIT or valid version.cmake. Version information will be invalid." )
ENDIF(NOT EXISTS "${CMAKE_SOURCE_DIR}/version.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/version.cmake")


SET(SUPERTUX_VERSION ${SUPERTUX_VERSION_STRING})

configure_file(version.h.in ${CMAKE_BINARY_DIR}/version.h )

SET_SOURCE_FILES_PROPERTIES(${CMAKE_BINARY_DIR}/version.h
                            PROPERTIES GENERATED true)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/src/supertux/main.cpp
                            PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/src/supertux/title_screen.cpp
                            PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/levels/misc/menu.stl.in ${CMAKE_BINARY_DIR}/data/levels/misc/menu.stl )

## Configure doxygen
# Adapted from https://tty1.net/blog/2014/cmake-doxygen_en.html
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Build API documentation using Doxygen" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)

  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
endif()

## Check platform-dependent build options

INCLUDE(ConfigureChecks)


## Also build external/squirrel

IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/squirrel/CMakeLists.txt)
  message(FATAL_ERROR "squirrel submodule is not checked out or ${CMAKE_CURRENT_SOURCE_DIR}/external/squirrel/CMakeLists.txt is missing")
ENDIF()

SET(SQUIRREL_PREFIX ${CMAKE_BINARY_DIR}/squirrel/)

# Define base arguments for all platforms
SET(SQUIRREL_CMAKE_ARGS
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_INSTALL_PREFIX=${SQUIRREL_PREFIX}
  -DINSTALL_INC_DIR=include
)

# Append Wii-specific arguments ONLY if building for Wii
IF(WII)
  LIST(APPEND SQUIRREL_CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DBUILD_SHARED_LIBS=OFF
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  )
  MESSAGE(STATUS "Configuring Squirrel for Wii cross-compilation")
ENDIF()

ExternalProject_Add(squirrel
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/squirrel/"
  CMAKE_ARGS ${SQUIRREL_CMAKE_ARGS}
)

IF(WIN32)
  ADD_LIBRARY(squirrel_lib SHARED IMPORTED)
  SET_TARGET_PROPERTIES(squirrel_lib PROPERTIES IMPORTED_LOCATION "${SQUIRREL_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}squirrel${CMAKE_SHARED_LIBRARY_SUFFIX}")
  SET_TARGET_PROPERTIES(squirrel_lib PROPERTIES IMPORTED_IMPLIB "${SQUIRREL_PREFIX}/lib/squirrel${CMAKE_LINK_LIBRARY_SUFFIX}")
  ADD_LIBRARY(sqstdlib_lib SHARED IMPORTED)
  SET_TARGET_PROPERTIES(sqstdlib_lib PROPERTIES IMPORTED_LOCATION "${SQUIRREL_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}sqstdlib${CMAKE_SHARED_LIBRARY_SUFFIX}")
  SET_TARGET_PROPERTIES(sqstdlib_lib PROPERTIES IMPORTED_IMPLIB "${SQUIRREL_PREFIX}/lib/sqstdlib${CMAKE_LINK_LIBRARY_SUFFIX}")

  #For debug run purposes
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/run_supertux.bat.in" "${PROJECT_BINARY_DIR}/run_supertux.bat")
ELSE()
  # Force explicit paths for Wii/Linux to avoid prefix/suffix confusion
  ADD_LIBRARY(squirrel_lib STATIC IMPORTED)
  SET_TARGET_PROPERTIES(squirrel_lib PROPERTIES IMPORTED_LOCATION "${SQUIRREL_PREFIX}/lib/libsquirrel_static.a")

  ADD_LIBRARY(sqstdlib_lib STATIC IMPORTED)
  SET_TARGET_PROPERTIES(sqstdlib_lib PROPERTIES IMPORTED_LOCATION "${SQUIRREL_PREFIX}/lib/libsqstdlib_static.a")
ENDIF()

INCLUDE_DIRECTORIES(SYSTEM ${SQUIRREL_PREFIX}/include)

## Some additional include paths

include_directories(${CMAKE_BINARY_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/obstack/)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/include/)

IF(WIN32)
  ADD_DEFINITIONS(-D_USE_MATH_DEFINES -DNOMINMAX)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

## Build list of sources for supertux binary

FILE(GLOB SUPERTUX_SOURCES_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} external/obstack/*.c)

FILE(GLOB SUPERTUX_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.cpp src/supertux/menu/*.cpp src/video/sdl/*.cpp)
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

IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.cpp)
  SET(SUPERTUX_SOURCES_CXX ${SUPERTUX_SOURCES_CXX} ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.cpp)
ENDIF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.cpp)

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

# Set C++20 globally (Works for all compilers)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set specific flags only for GCC or Clang
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  SET(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  SET(CMAKE_CXX_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  SET(CMAKE_C_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  SET(CMAKE_LD_FLAGS_PROFILE "-lgmon" CACHE STRING "Profile flags")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -funit-at-a-time")

  IF(WERROR)
    ADD_DEFINITIONS(-Werror)
  ELSE(WERROR)
    REMOVE_DEFINITIONS(-Werror)
  ENDIF(WERROR)

  IF(WARNINGS)
    # temporarily disabled:
    #   -Wsign-conversion -Wfloat-equal -Wconversion -Wundef -Wswitch-default
    #   -Wswitch-enum -Wsign-promo -Wcast-qual -Woverloaded-virtual -Wmissing-format-attribute
    #   -Wold-style-cast -Wpadded -Wabi -Winline -Wunsafe-loop-optimizations -Wstrict-overflow=5
    # fails on MinGW:
    #   -ansi
    # fails on clang:
    #   -Wlogical-op -Wstrict-null-sentinel
    SET(SUPERTUX2_EXTRA_WARNING_FLAGS "-fdiagnostics-show-option -pedantic -Wno-long-long -Wcast-align -Wdisabled-optimization -Winit-self -Winvalid-pch -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wredundant-decls -Wstack-protector -Wformat=2 -Weffc++ -Wctor-dtor-privacy  -Wno-unused-parameter -Wshadow -Wnon-virtual-dtor -Wcast-qual")
  ENDIF(WARNINGS)
endif()

## Some additional compiler switches
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  ADD_DEFINITIONS(-DMACOSX)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


## On Windows, add an icon

IF(WIN32)
#  SET(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc)
  IF(MINGW)
    ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o
    COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons -i${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc -o ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
    SET(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
  ENDIF(MINGW)
ENDIF(WIN32)

## Build miniswig and generate miniswig wrapper

OPTION(GENERATE_WRAPPER "Build miniswig and generate the wrapper" OFF)
IF(GENERATE_WRAPPER)
  ADD_SUBDIRECTORY(tools/miniswig)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.hpp
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && ${CMAKE_CXX_COMPILER}
    ARGS -x "c++" -E -CC -DSCRIPTING_API src/scripting/wrapper.interface.hpp -o ${CMAKE_CURRENT_BINARY_DIR}/miniswig.tmp -I${CMAKE_CURRENT_SOURCE_DIR}/src
    COMMAND tools/miniswig/miniswig
    ARGS --input miniswig.tmp --output-cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.cpp --output-hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.hpp --module supertux --select-namespace scripting
    DEPENDS tools/miniswig/miniswig
    IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.interface.hpp
  )
ENDIF(GENERATE_WRAPPER)

## Generate supertux executable in the right place

#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

## Add target for supertux binary

add_library(supertux2_c OBJECT ${SUPERTUX_SOURCES_C})
add_library(supertux2_lib STATIC ${CMAKE_BINARY_DIR}/version.h ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_RESOURCES} $<TARGET_OBJECTS:supertux2_c>)
IF(WIN32)
  add_executable(supertux2 WIN32 src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc)
  TARGET_LINK_LIBRARIES(supertux2 ${SDL2MAIN_LIBRARIES})
ELSE()
  add_executable(supertux2 src/main.cpp)
ENDIF(WIN32)
target_link_libraries(supertux2 supertux2_lib)
set_target_properties(supertux2 PROPERTIES OUTPUT_NAME supertux2)
set_target_properties(supertux2_lib PROPERTIES OUTPUT_NAME supertux2)
set_target_properties(supertux2_lib PROPERTIES COMPILE_FLAGS "${SUPERTUX2_EXTRA_WARNING_FLAGS}")

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

ADD_DEPENDENCIES(supertux2_lib squirrel)

TARGET_INCLUDE_DIRECTORIES(supertux2_lib SYSTEM PUBLIC ${SDL2_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(supertux2_lib SYSTEM PUBLIC ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${SDL2_LIBRARIES})
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${SDL2IMAGE_LIBRARIES})

TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC squirrel_lib)
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC sqstdlib_lib)
TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC sexp)
IF(ENABLE_OPENAL)
  TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OPENAL_LIBRARY})
  TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OGGVORBIS_LIBRARIES})
ELSE()
  TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${SDL2MIXER_LIBRARIES})
ENDIF()

IF(HAVE_OPENGL)
  # Use OPENGL_LIBRARIES if available (Wii), otherwise fallback to OPENGL_LIBRARY (Desktop)
  IF(DEFINED OPENGL_LIBRARIES)
    TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OPENGL_LIBRARIES})
  ELSE()
    TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${OPENGL_LIBRARY})
  ENDIF()
  IF(GLBINDING_ENABLED)
    TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${GLBINDING_LIBRARIES})
  ELSE()
    # Only link GLEW on non-Wii platforms
    IF(NOT WII AND GLEW_FOUND)
      TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC ${GLEW_LIBRARY})
    ENDIF()
  ENDIF()
ENDIF(HAVE_OPENGL)

if(BUILD_TESTS)
  find_package(Threads REQUIRED)

  # build gtest
  # ${CMAKE_CURRENT_SOURCE_DIR} in include_directories is needed to generate -isystem instead of -I flags
  add_library(gtest_main STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/googletest/src/gtest_main.cc)
  target_include_directories(gtest_main SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/googletest/include/)
  add_library(gtest STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/googletest/src/gtest-all.cc)
  target_include_directories(gtest SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/googletest/)
  target_include_directories(gtest SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/googletest/include/)

  # build SuperTux tests
  file(GLOB TEST_SUPERTUX_SOURCES tests/*.cpp)
  add_executable(test_supertux2 ${TEST_SUPERTUX_SOURCES})
  target_compile_options(test_supertux2 PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_supertux2
    gtest gtest_main
    supertux2_lib
    ${CMAKE_THREAD_LIBS_INIT})

  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
  add_test(NAME test_supertux2
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test_supertux2)
endif()

## Install stuff

OPTION(DISABLE_CPACK_BUNDLING "Build an .app bundle without CPack" OFF)
IF(WIN32 AND NOT UNIX)

  SET(INSTALL_SUBDIR_BIN "bin" CACHE STRING "Installation subdir for binaries")
  SET(INSTALL_SUBDIR_SHARE "data" CACHE STRING "Installation subdir for data")
  SET(INSTALL_SUBDIR_DOC "doc" CACHE STRING "Installation subdir for docs")

  INSTALL(FILES ${DLLS} DESTINATION ${INSTALL_SUBDIR_BIN})

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.ico DESTINATION ".")

  OPTION(PACKAGE_VCREDIST "Package the VCREDIST libraries with the program" OFF)

  IF(PACKAGE_VCREDIST)
    include(InstallRequiredSystemLibraries)
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
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND DISABLE_CPACK_BUNDLING)

  SET(INFOPLIST_CFBUNDLEEXECUTABLE "supertux2")

  SET(INSTALL_SUBDIR_BIN "SuperTux.app/Contents/MacOS" CACHE STRING "Installation subdir for binaries")
  SET(INSTALL_SUBDIR_SHARE "SuperTux.app/Contents/Resources/data" CACHE STRING "Installation subdir for data")
  SET(INSTALL_SUBDIR_DOC "SuperTux.app/Contents/Resources" CACHE STRING "Installation subdir for docs")

  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/info.plist.in ${CMAKE_BINARY_DIR}/tools/darwin/info.plist)
  INSTALL(FILES ${CMAKE_BINARY_DIR}/tools/darwin/info.plist DESTINATION "SuperTux.app/Contents/")
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/receipt DESTINATION "SuperTux.app/Contents/_MASReceipt/")

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.icns DESTINATION "SuperTux.app/Contents/Resources/")

ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND DISABLE_CPACK_BUNDLING)

  SET(INSTALL_SUBDIR_BIN "games" CACHE STRING "Installation subdir for binaries")
  SET(INSTALL_SUBDIR_SHARE "share/games/supertux2" CACHE STRING "Installation subdir for data")
  SET(INSTALL_SUBDIR_DOC "share/doc/supertux2" CACHE STRING "Installation subdir for docs")

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/supertux2.desktop DESTINATION "share/applications")

  SET(APPS "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_SUBDIR_BIN}/supertux2")

  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.xpm DESTINATION "share/pixmaps/")

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND DISABLE_CPACK_BUNDLING)
ENDIF(WIN32 AND NOT UNIX)

INSTALL(TARGETS supertux2 DESTINATION ${INSTALL_SUBDIR_BIN})

IF(WIN32)
  GET_PROPERTY(SQUIRREL_LIB_PATH TARGET squirrel_lib PROPERTY IMPORTED_LOCATION)
  GET_PROPERTY(SQSTDLIB_LIB_PATH TARGET sqstdlib_lib PROPERTY IMPORTED_LOCATION)
  INSTALL(FILES ${SQUIRREL_LIB_PATH} ${SQSTDLIB_LIB_PATH} DESTINATION ${INSTALL_SUBDIR_BIN})
ENDIF()

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/INSTALL.md ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${INSTALL_SUBDIR_DOC})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/credits.stxt DESTINATION ${INSTALL_SUBDIR_SHARE})

# Install worldmap and tile definitions
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/ice_world.strf ${CMAKE_CURRENT_SOURCE_DIR}/data/tiles.strf ${CMAKE_CURRENT_SOURCE_DIR}/data/worldmap.strf DESTINATION ${INSTALL_SUBDIR_SHARE})

# Install Linux desktop integration files (Appdata and Man page)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/supertux2.appdata.xml DESTINATION "share/appdata" )
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man/man6/supertux2.6 DESTINATION "share/man/man6" )

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/images
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/music
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/objects
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/scripts
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/speech
                  ${CMAKE_CURRENT_SOURCE_DIR}/data/sounds
                  DESTINATION ${INSTALL_SUBDIR_SHARE})

IF(CMAKE_BUILD_TYPE MATCHES Release)
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
                    DESTINATION ${INSTALL_SUBDIR_SHARE}
                    PATTERN "data/levels/test" EXCLUDE
                    PATTERN "data/levels/test_old" EXCLUDE
                    PATTERN "data/levels/incubator" EXCLUDE
                    PATTERN "data/levels/misc/menu.stl.in" EXCLUDE)
ELSE()
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
                    DESTINATION ${INSTALL_SUBDIR_SHARE}
                    PATTERN "data/levels/misc/menu.stl.in" EXCLUDE)
ENDIF()

INSTALL(FILES "${CMAKE_BINARY_DIR}/data/levels/misc/menu.stl" DESTINATION "${INSTALL_SUBDIR_SHARE}/levels/misc")

## Create config.h now that INSTALL_SUBDIR_* have been set.

configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h )

## Configure main menu logo
IF(CMAKE_BUILD_TYPE MATCHES Release)
  SET(LOGO_FILE "logo_final.sprite")
ELSE()
  SET(LOGO_FILE "logo.sprite")
ENDIF()
configure_file(data/levels/misc/menu.stl.in ${CMAKE_BINARY_DIR}/data/levels/misc/menu.stl )


## CPack/Installation-specific stuff

INCLUDE(InstallRequiredSystemLibraries)
SET(DIRS ${CMAKE_CURRENT_BINARY_DIR}/external/squirrel)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT DISABLE_CPACK_BUNDLING)
  SET(INFOPLIST_CFBUNDLEEXECUTABLE "SuperTux")

  FIND_PACKAGE(png)
  foreach (_file ${PNG_LIBRARIES})
    get_filename_component(_resolvedFile "${_file}" REALPATH)
    get_filename_component(_name "${_file}" NAME)
    INSTALL(FILES ${_resolvedFile} DESTINATION "MacOS" RENAME ${_name})
  endforeach()
  FIND_PACKAGE(jpeg)
  foreach (_file ${JPEG_LIBRARIES})
    get_filename_component(_resolvedFile "${_file}" REALPATH)
    get_filename_component(_name "${_file}" NAME)
    INSTALL(FILES ${_resolvedFile} DESTINATION "MacOS" RENAME ${_name})
  endforeach()

  INSTALL(CODE "
     if(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/\" MATCHES \".*\\\\.app.*\")
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
     endif()
     ")

  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/info.plist.in" "${CMAKE_BINARY_DIR}/tools/darwin/info.plist")

  SET(CPACK_BUNDLE_NAME "SuperTux")
  SET(CPACK_BUNDLE_PLIST "${CMAKE_BINARY_DIR}/tools/darwin/info.plist")
  SET(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.icns")
  SET(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/startup.sh")
  SET(CPACK_DMG_VOLUME_NAME "SuperTux ${SUPERTUX_VERSION_STRING}")
  SET(CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/DS_Store")
  SET(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/background.png")
ENDIF()



SET(CPACK_PACKAGE_NAME "SuperTux")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Jump'n'Run Game featuring Tux")
SET(CPACK_PACKAGE_VENDOR "SuperTux Devel Team")
SET(CPACK_PACKAGE_CONTACT "SuperTux Devel Team <supertux-devel@lists.lethargik.org>")
SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;${CMAKE_BINARY_DIR};/\\\\..*")
SET(CPACK_DEBIAN_PACKAGE_NAME "supertux2")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.5), libgcc1 (>= 1:4.1), libgl1-mesa-glx | libgl1, libogg0 (>= 1.1.3), libopenal0a, libsdl-image1.2 (>= 1.2.5), libsdl1.2debian (>= 1.2.10-1), libstdc++6 (>= 4.1.2), libvorbis0a (>= 1.1.2), libvorbisfile3 (>= 1.1.2)")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Classic 2D jump 'n run sidescroller with Tux\n SuperTux is a classic 2D jump 'n run sidescroller game in a similar\n style like the original SuperMario games. This release of SuperTux\n features 9 enemies, 26 playable levels, software and OpenGL rendering\n modes, configurable joystick and keyboard input, new music and\n completely redone graphics.\n .\n This is a development snapshot of SuperTux. It may suffer from\n critical bugs and has not been fully tested. \n .\n Homepage: http://supertux.lethargik.org/")
SET(CPACK_DEBIAN_PACKAGE_SECTION "games")
SET(CPACK_RPM_PACKAGE_NAME "supertux2")
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
SET(CPACK_PACKAGE_EXECUTABLES "supertux2" "SuperTux")
IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "supertux2")
  SET(CPACK_NSIS_DISPLAY_NAME "SuperTux")
  SET(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  SET(CPACK_WIX_PROGRAM_MENU_FOLDER "")
  SET(CPACK_WIX_UPGRADE_GUID "93E16F4E-0A68-422A-8ADC-47BE5B9433B2")
  SET(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.ico")

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
  CMAKE_OSX_ARCHITECTURES
  CMAKE_OSX_SYSROOT
)

MARK_AS_ADVANCED(
  APPDATADIR
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

# Add Wii-specific libraries and fix link order
IF(WII)
  # Explicitly list all dependencies in the correct order for static linking
  # Order: SDL2_mixer -> Audio Codecs -> SDL2_image -> Image Codecs -> SDL2 -> OpenGX -> Wii System Libs
  TARGET_LINK_LIBRARIES(supertux2_lib PUBLIC
    -lSDL2_mixer
    -lmodplug -lopusfile -lopus -lvorbisidec -logg -lmad
    -lSDL2_image
    -lpng -ljpeg -lz
    -lSDL2
    ${OPENGL_LIBRARIES}
    -lfreetype -lfat -lwiiuse -lbte -lwiikeyboard -laesnd -logc -lm
  )

  # Increase Stack Size to 2MB (2097152 bytes)
  # This is critical! Without it, the game overwrites its own memory.
  SET_TARGET_PROPERTIES(supertux2 PROPERTIES LINK_FLAGS "-Wl,--defsym,__stack_size=2097152")

  # Convert ELF to DOL after build using elf2dol
  ADD_CUSTOM_COMMAND(TARGET supertux2 POST_BUILD
      COMMAND elf2dol $<TARGET_FILE:supertux2> ${CMAKE_BINARY_DIR}/boot.dol
      COMMENT "Converting ELF to DOL for Wii..."
  )

  MESSAGE(STATUS "Wii build configured:")
  MESSAGE(STATUS "  - Stack size: 2MB")
  MESSAGE(STATUS "  - Output: boot.dol (for sd:/apps/supertux2/)")
  MESSAGE(STATUS "  - NPOT textures: DISABLED (forced POT)")
  MESSAGE(STATUS "  - Lightmap mode: ROUNDTRIP")
ENDIF()

# EOF
