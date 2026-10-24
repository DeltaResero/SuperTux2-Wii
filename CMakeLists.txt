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
cmake_minimum_required(VERSION 3.16...4.0)

## Project name to use as command prefix.

project(SUPERTUX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake)
include(CheckCXXCompilerFlag)

set(BUILD_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(BUILD_CONFIG_DATA_DIR "${CMAKE_BINARY_DIR}/data")
## Add definitions
if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  add_definitions(-DRELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG)
endif()

option(REMOVE_QUIT_BUTTON "Remove the option to quit the game (useful on mobile devices)" OFF)

set(SUPERTUX_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(VCPKG_BUILD OFF CACHE BOOL "Use dependencies installed via vcpkg (not dependency package)")
set(VCPKG_APPLOCAL_DEPS ${VCPKG_BUILD} BOOL)
## These variables enable MSVC to find libraries located in "dependencies{32|64}"
if(WIN32)
  if(NOT VCPKG_BUILD)
    ## Store path of dependecy folder
    set(DEPENDENCY_FOLDER "${PROJECT_SOURCE_DIR}/dependencies")
  endif()

  ## To test if the host (not the build) is x64:
  ## "$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "AMD64"
  if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(WIN64 ON)
    if(NOT VCPKG_BUILD)
      set(DEPENDENCY_FOLDER "${DEPENDENCY_FOLDER}64")
    endif()
  else(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(WIN64 OFF)
    if(NOT VCPKG_BUILD)
      set(DEPENDENCY_FOLDER "${DEPENDENCY_FOLDER}32")
    endif()
  endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

  if(WIN64)
    set(SUPERTUX_SYSTEM_NAME win64)
  else(WIN64)
    set(SUPERTUX_SYSTEM_NAME win32)
  endif(WIN64)

  if(NOT VCPKG_BUILD)
    set(ENV{PATH} "$ENV{PATH};${DEPENDENCY_FOLDER}/include")
    set(ENV{LIB} "${DEPENDENCY_FOLDER}/lib")
    set(ENV{OPENALDIR} "${DEPENDENCY_FOLDER}")
  endif()

  ## Enable multi-processor compilation (faster)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  endif(MSVC)

  ## And shut up about unsafe stuff
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  ## Add an icon
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/icon_rc.template" "${PROJECT_BINARY_DIR}/tmp/icon.rc")

  ## Find all used libraries
  if(NOT VCPKG_BUILD)
    file(GLOB DLLS "${DEPENDENCY_FOLDER}/dll/*")
  endif()
endif(WIN32)

option(ENABLE_TOUCHSCREEN_SUPPORT "Enable on-screen controls and event detection for touchscreen devices" OFF)
option(ENABLE_CONSOLE "Build the developer console, opened with a key in developer mode" OFF)

## Add lots of dependencies to compiler switches

find_package(glm QUIET)
if(glm_FOUND)
  if(TARGET glm::glm)
    message(STATUS "Found glm")
  else()
    # fallback for old glm version in EmuELEC and Nix
    get_target_property(GLM_INCLUDE_DIR glm INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "Found glm: ${GLM_INCLUDE_DIR}")
    add_library(glm::glm INTERFACE IMPORTED)
    set_target_properties(glm::glm PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE_DIR})
  endif()
else()
  # fallback for old glm version in UBPorts
  find_path(GLM_INCLUDE_DIR
    NAMES glm/glm.hpp
    PATHS ${GLM_ROOT_DIR}/include)
  if(NOT GLM_INCLUDE_DIR)
    message(FATAL_ERROR "glm library missing")
  endif()

  message(STATUS "Found glm: ${GLM_INCLUDE_DIR}")
  add_library(glm::glm INTERFACE IMPORTED)
  set_target_properties(glm::glm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE_DIR})
endif()

if(WIN32)
  if(VCPKG_BUILD)
    find_package(SDL2 CONFIG REQUIRED)
    find_library(SDL2IMAGE_LIBRARIES SDL2_image)
    find_library(SDL2MAIN_LIBRARIES SDL2main)
    message(STATUS "SDL2_INCLUDE_DIR is ${SDL2_INCLUDE_DIRS}")
  else(VCPKG_BUILD)
    find_path(SDL2_INCLUDE_DIRS NAMES SDL.h PATHS "${DEPENDENCY_FOLDER}/include/SDL2")
    find_path(SDL2IMAGE_INCLUDE_DIRS NAMES SDL_image.h PATHS "${DEPENDENCY_FOLDER}/include/SDL2_image")
    find_library(SDL2_LIBRARIES NAMES SDL2 PATHS "${DEPENDENCY_FOLDER}/lib")
    find_library(SDL2IMAGE_LIBRARIES NAMES SDL2_image PATHS "${DEPENDENCY_FOLDER}/lib")
    find_library(SDL2MAIN_LIBRARIES NAMES SDL2main PATHS "${DEPENDENCY_FOLDER}/lib")
  endif(VCPKG_BUILD)

else(WIN32)

  if (VCPKG_BUILD)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-image CONFIG REQUIRED)
    find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)
    find_path(SDL2IMAGE_INCLUDE_DIRS SDL2/SDL_image.h)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS}/SDL2)
    set(SDL2IMAGE_INCLUDE_DIRS ${SDL2IMAGE_INCLUDE_DIRS}/SDL2)
    find_library(SDL2_LIBRARIES SDL2)
    find_library(SDL2IMAGE_LIBRARIES SDL2_image)
    message(STATUS "SDL2_INCLUDE_DIR is ${SDL2_INCLUDE_DIRS}")
  elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32-bit
    find_library(SDL2_LIBRARIES SDL2)
    find_library(SDL2IMAGE_LIBRARIES SDL2_image)
    find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)
    find_path(SDL2IMAGE_INCLUDE_DIRS SDL2/SDL_image.h)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS}/SDL2)
    set(SDL2IMAGE_INCLUDE_DIRS ${SDL2IMAGE_INCLUDE_DIRS}/SDL2)
    message(STATUS "SDL LIBS: ${SDL2_LIBRARIES}")
    message(STATUS "SDL INCL: ${SDL2_INCLUDE_DIRS}")
  else()
    # 64-bit
    include(FindPkgConfig)
    # 2.0.18 for the SDL renderer's vsync switch and its float drawing calls
    pkg_search_module(SDL2 REQUIRED sdl2>=2.0.18)
    pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
  endif()
endif(WIN32)
set(HAVE_SDL TRUE)

option(IS_SUPERTUX_RELEASE "Build as official SuperTux release" OFF)
option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_TESTS_M32 "Also run the squirrel samples at 32-bit integer width" OFF)
option(ENABLE_OPENGL "Enable OpenGL support" ON)
option(ENABLE_OPENGLES2 "Enable OpenGLES2 support" OFF)
# Only Windows' own OpenGL library stops short of the functions the renderer calls
if(WIN32)
  set(ENABLE_GLEW_DEFAULT ON)
else()
  set(ENABLE_GLEW_DEFAULT OFF)
endif()
option(ENABLE_GLEW "Use GLEW to load OpenGL functions" ${ENABLE_GLEW_DEFAULT})
option(GLBINDING_ENABLED "Use glbinding instead of GLEW" OFF)
option(GLBINDING_DEBUG_OUTPUT "Enable glbinding debug output for each called OpenGL function" OFF)
option(ENABLE_NPOT_TEXTURES "Use textures of any size even when the driver doesn't list the extension" OFF)
set(TEXTURE_ALIGNMENT 1 CACHE STRING "Round texture sides up to a multiple of this many pixels")
if(NOT TEXTURE_ALIGNMENT MATCHES "^[1-9][0-9]*$")
  message(FATAL_ERROR "TEXTURE_ALIGNMENT must be a positive whole number, got '${TEXTURE_ALIGNMENT}'")
endif()
option(ENABLE_LIGHTMAP_FBO "Draw the lightmap through a framebuffer instead of copying it from the screen" OFF)

if(ENABLE_OPENGL)
  if(ENABLE_OPENGLES2)
    pkg_check_modules(GLESV2 REQUIRED glesv2)
    set(HAVE_OPENGL TRUE)
    set(OPENGL_INCLUDE_DIR  ${GLESV2_INCLUDE_DIRS})
    set(OPENGL_LIBRARY ${GLESV2_LIBRARIES})
    add_definitions(-DUSE_OPENGLES2)
  else()
    set(OpenGL_GL_PREFERENCE "LEGACY")
    find_package(OpenGL)
    if(OPENGL_FOUND)
      include_directories(SYSTEM ${OPENGL_INCLUDE_DIR})
      set(HAVE_OPENGL TRUE)
    endif(OPENGL_FOUND)

    if(GLBINDING_ENABLED)
      find_package(GLBINDING REQUIRED)
      if(GLBINDING_FOUND)
        include_directories(SYSTEM ${GLBINDING_INCLUDES})
        add_definitions(-DUSE_GLBINDING)
        IF (GLBINDING_DEBUG_OUTPUT)
          add_definitions(-DUSE_GLBINDING_DEBUG_OUTPUT)
        endif()
      endif()
    elseif(ENABLE_GLEW)
      find_package(GLEW REQUIRED)
      if(GLEW_FOUND)
        include_directories(SYSTEM ${GLEW_INCLUDE_DIR})
        set(HAVE_GLEW TRUE)
      endif(GLEW_FOUND)
    endif()
  endif()
endif(ENABLE_OPENGL)

option(ENABLE_OPENAL "Build the OpenAL sound backend" ON)
if(ENABLE_OPENAL)
  if(VCPKG_BUILD)
    find_package(OpenAL CONFIG REQUIRED)
  else()
    find_package(OpenAL REQUIRED)
  endif()
  include_directories(SYSTEM ${OPENAL_INCLUDE_DIR})
endif()

option(ENABLE_SDL_MIXER "Build the SDL_mixer sound backend, picked with --audio-backend sdl" OFF)
if(ENABLE_SDL_MIXER)
  if(VCPKG_BUILD)
    find_package(SDL2_mixer CONFIG REQUIRED)
    set(SDL2MIXER_TARGET SDL2_mixer::SDL2_mixer)
  else()
    include(FindPkgConfig)
    pkg_check_modules(SDL2MIXER REQUIRED IMPORTED_TARGET SDL2_mixer>=2.0.0)
    set(SDL2MIXER_TARGET PkgConfig::SDL2MIXER)
  endif()
endif()

if(NOT ENABLE_OPENAL AND NOT ENABLE_SDL_MIXER)
  message(FATAL_ERROR "Enable at least one of ENABLE_OPENAL and ENABLE_SDL_MIXER")
endif()

find_package(OggVorbis REQUIRED)
include_directories(SYSTEM ${VORBIS_INCLUDE_DIR})

if(VCPKG_BUILD)
  find_package(FLAC CONFIG REQUIRED)
  set(FLAC_TARGET FLAC::FLAC)
else()
  include(FindPkgConfig)
  pkg_check_modules(FLAC REQUIRED IMPORTED_TARGET flac)
  set(FLAC_TARGET PkgConfig::FLAC)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  ## Find revision of WC
  mark_as_advanced(GIT_EXECUTABLE)
  find_program(GIT_EXECUTABLE git)
  if(NOT GIT_EXECUTABLE EQUAL "GIT_EXECUTABLE-NOTFOUND")
    include(GetGitRevisionDescription)
    git_describe(VERSION_STRING_GIT "--tags" "--match" "?[0-9]*.[0-9]*.[0-9]*")
    string(REPLACE "v" "" VERSION_LIST ${VERSION_STRING_GIT})
    string(REGEX REPLACE "[^\\-]+\\-([0-9]+)\\-.*" "\\1" VERSION_NUMBER_GIT "${VERSION_LIST}")
    string(REGEX REPLACE "(-|_|\\.)" ";" VERSION_LIST ";${VERSION_LIST}")
  endif()
endif(EXISTS "${CMAKE_SOURCE_DIR}/.git")

get_filename_component(BASEDIR ${CMAKE_SOURCE_DIR} NAME)
if("${VERSION_LIST}" STREQUAL "")
  if(${BASEDIR} MATCHES "supertux2-[0-9\\.]*")
    string(REGEX REPLACE "(\\.|_|-)" ";" VERSION_LIST ${BASEDIR})
  endif()
endif()

file(GLOB ORIG_TGZ ../*.orig.tar.gz)
if("${VERSION_LIST}" STREQUAL "" AND (NOT "${ORIG_TGZ}" STREQUAL ""))
  get_filename_component(BASEDIR ${ORIG_TGZ} NAME)
  string(REGEX REPLACE "(\\.|_|-)" ";" VERSION_LIST ${BASEDIR})
endif()

list(LENGTH VERSION_LIST VERSION_LIST_SIZE)

if(${VERSION_LIST_SIZE} GREATER 0)
  list(GET VERSION_LIST 1 MAJOR_VERSION_GIT)
  list(GET VERSION_LIST 2 MINOR_VERSION_GIT)
  list(GET VERSION_LIST 3 PATCH_VERSION_GIT)

  if("${VERSION_STRING_GIT}" STREQUAL "")
    set(VERSION_STRING_GIT "${MAJOR_VERSION_GIT}.${MINOR_VERSION_GIT}.${PATCH_VERSION_GIT}")
  endif()

  configure_file("${CMAKE_SOURCE_DIR}/version.cmake.in" "${CMAKE_SOURCE_DIR}/version.cmake")
endif()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/version.cmake")
  message( SEND_ERROR "Could not find GIT or valid version.cmake. Version information will be invalid." )
endif(NOT EXISTS "${CMAKE_SOURCE_DIR}/version.cmake")
include("${CMAKE_SOURCE_DIR}/version.cmake")


set(SUPERTUX_VERSION ${SUPERTUX_VERSION_STRING})

configure_file(version.h.in ${CMAKE_BINARY_DIR}/version.h )

set_source_files_properties(${CMAKE_BINARY_DIR}/version.h
  PROPERTIES GENERATED true)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/supertux/main.cpp
  PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/supertux/title_screen.cpp
  PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")

# Include altivec wrapper on ppc
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^ppc.*")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/ppc)
endif()


## Also build simplesquirrel and the squirrel it binds

# Built here: their CMake installs a shared library we don't ship, and a subdirectory misses the flags set below
set(SQUIRREL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/squirrel)
set(SIMPLESQUIRREL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/simplesquirrel)

file(GLOB SQUIRREL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SQUIRREL_DIR}/squirrel/*.cpp)
file(GLOB SQSTDLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SQUIRREL_DIR}/sqstdlib/*.cpp)
file(GLOB SIMPLESQUIRREL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SIMPLESQUIRREL_DIR}/source/*.cpp)

add_library(squirrel STATIC ${SQUIRREL_SOURCES})
add_library(sqstdlib STATIC ${SQSTDLIB_SOURCES})
add_library(simplesquirrel STATIC ${SIMPLESQUIRREL_SOURCES})

# Their headers compile inside our own files, so they're system headers to our warning flags
target_include_directories(squirrel SYSTEM PUBLIC ${SQUIRREL_DIR}/include)
target_include_directories(simplesquirrel SYSTEM PUBLIC ${SIMPLESQUIRREL_DIR}/include)
target_link_libraries(sqstdlib PUBLIC squirrel)
target_link_libraries(simplesquirrel PUBLIC squirrel sqstdlib)

# _SQ64 widens SQInteger, so every file that sees squirrel.h needs it, not just the library's own
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions(squirrel PUBLIC _SQ64)
endif()

if(WIN32)
  #For debug run purposes
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/run_supertux.bat.in" "${PROJECT_BINARY_DIR}/run_supertux.bat")
endif()

## SDL_ttf, the one installed rather than a bundled copy
if(VCPKG_BUILD)
  find_package(SDL2_ttf CONFIG REQUIRED)
  set(SDL2TTF_TARGET SDL2_ttf::SDL2_ttf)
else()
  include(FindPkgConfig)
  pkg_check_modules(SDL2TTF REQUIRED IMPORTED_TARGET SDL2_ttf>=2.0.14)
  set(SDL2TTF_TARGET PkgConfig::SDL2TTF)
endif()

## Some additional include paths

include_directories(${CMAKE_BINARY_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/include/)

if(WIN32)
  add_definitions(-DNOMINMAX)
  add_definitions(-DWIN32)
endif(WIN32)

## Build list of sources for supertux binary

# This glob reaches src/lib/*.cpp too, so nothing vendored may sit at that depth
file(GLOB SUPERTUX_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.cpp src/supertux/menu/*.cpp src/video/sdl/*.cpp src/video/null/*.cpp)
if(NOT ENABLE_OPENAL)
  list(REMOVE_ITEM SUPERTUX_SOURCES_CXX src/audio/openal_device.cpp src/audio/openal_sound_source.cpp
                                        src/audio/stream_sound_source.cpp)
endif()
if(NOT ENABLE_SDL_MIXER)
  list(REMOVE_ITEM SUPERTUX_SOURCES_CXX src/audio/sdl_mixer_device.cpp src/audio/sdl_music.cpp
                                        src/audio/sdl_samples.cpp src/audio/sdl_sound_source.cpp
                                        src/audio/sdl_stream.cpp src/audio/sdl_voice.cpp)
endif()
file(GLOB SUPERTUX_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROJECT_BINARY_DIR}/tmp/*.rc")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/CMakeLists.txt)
  message(FATAL_ERROR "sexp-cpp submodule is not checked out or ${CMAKE_CURRENT_SOURCE_DIR}/external/sexp-cpp/CMakeLists.txt is missing")
endif()
file(GLOB SEXP_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} external/sexp-cpp/src/*.cpp)
add_library(sexp STATIC ${SEXP_SOURCES_CXX})

if(HAVE_OPENGL)
  file(GLOB SUPERTUX_OPENGL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/video/gl/*.cpp)
  set(SUPERTUX_SOURCES_CXX ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_OPENGL_SOURCES})
endif(HAVE_OPENGL)

## Sort source lists to have deterministic linking order
list(SORT SUPERTUX_SOURCES_CXX)
list(SORT SUPERTUX_RESOURCES)

## Compile everything at once (roughly equivalent to cat *.cpp | gcc)

option(COMPILE_AMALGATION "Compile all the files together at once (experimental)" OFF)
if(COMPILE_AMALGATION)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp "// Includes all source files of the project\n")
  foreach(ST_FILE ${SUPERTUX_SOURCES_CXX})
    file(RELATIVE_PATH CXX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src "${CMAKE_CURRENT_SOURCE_DIR}/${ST_FILE}")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp "#include \"${CXX_FILE}\"\n")
  endforeach(ST_FILE)
  ## Build instead of CXX sources
  set(SUPERTUX_SOURCES_CXX ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
else(COMPILE_AMALGATION)
  if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
  endif(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
endif(COMPILE_AMALGATION)

## Debug options

option(WERROR "Stop on first compiler warning" OFF)
option(WARNINGS "Enable long list of warnings for compiler to check" OFF)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  set(CMAKE_CXX_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  set(CMAKE_C_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  set(CMAKE_LD_FLAGS_PROFILE "-lgmon" CACHE STRING "Profile flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  if(WERROR)
    add_definitions(-Werror)
  else(WERROR)
    remove_definitions(-Werror)
  endif(WERROR)
  if(WARNINGS)
    # temporarily disabled:
    #   -Wsign-conversion -Wfloat-equal -Wundef -Wswitch-default
    #   -Wswitch-enum -Wsign-promo -Wcast-qual -Woverloaded-virtual -Wmissing-format-attribute
    #   -Wpadded -Wabi -Winline -Wunsafe-loop-optimizations -Wstrict-overflow=5
    # fails on MinGW:
    #   -ansi
    if(CMAKE_COMPILER_IS_GNUCXX)
      string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
        "-fdiagnostics-show-option "
        "-pedantic "
        "-Wno-long-long "
        "-Wcast-align "
        "-Wdisabled-optimization "
        "-Winit-self -Winvalid-pch "
        "-Wmissing-include-dirs "
        "-Wmissing-noreturn "
        "-Wpacked -Wredundant-decls "
        "-Wformat=2 "
        "-Weffc++ "
        "-Wctor-dtor-privacy "
        "-Wno-unused-parameter "
        "-Wshadow "
        "-Wnon-virtual-dtor "
        "-Wcast-qual "
        "-Wold-style-cast "
        "-Wzero-as-null-pointer-constant "
        "-Wconversion "
        "-Wlogical-op "
        "-Wstrict-null-sentinel "
        "-Wsuggest-override "
        )
      if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8)
        string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
          "${SUPERTUX2_EXTRA_WARNING_FLAGS} "
          "-Wint-in-bool-context "
          )
      endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
        "${SUPERTUX2_EXTRA_WARNING_FLAGS} "
        "-Weverything "

        # flags that we deliberately ignore
        "-Wno-c++98-compat "
        "-Wno-c++98-compat-pedantic "
        "-Wno-float-equal "
        "-Wno-padded "
        "-Wno-weak-vtables "
        "-Wno-disabled-macro-expansion "
        "-Wno-documentation "
        "-Wno-reserved-id-macro "
        "-Wno-sign-conversion "

        # warnings that should probably be fixed in code
        "-Wno-documentation-unknown-command "
        "-Wno-inconsistent-missing-destructor-override "
        "-Wno-deprecated-dynamic-exception-spec "
        "-Wno-deprecated "
        "-Wno-switch-enum "
        "-Wno-covered-switch-default "
        "-Wno-exit-time-destructors "
        "-Wno-global-constructors "
        "-Wno-duplicate-enum "
        "-Wno-unreachable-code "
      )

      # This snipped adapted from AOMediaCodec cmakefile
      # The detection of cross compilation by -Wpoison-system-directories has false positives on macOS because
      # --sysroot is implicitly added. Turn the warning off.
      if(NOT DEFINED HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
          check_cxx_compiler_flag(-Wpoison-system-directories HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
      endif()
      if(HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
        string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
          "${SUPERTUX2_EXTRA_WARNING_FLAGS} "
          "-Wno-poison-system-directories ")
      endif()
    endif()
  endif(WARNINGS)
endif()

## Some additional compiler switches
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif()

## On Windows, add an icon

if(WIN32)
  #  set(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc)
  if(MINGW)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o
      COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons -i${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc -o ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
    set(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
  endif(MINGW)
endif(WIN32)

## Generate supertux executable in the right place

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

## Add target for supertux binary

add_library(supertux2_lib STATIC ${CMAKE_BINARY_DIR}/version.h ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_RESOURCES} ${SUPERTUX_SOURCES_C})

target_link_libraries(supertux2_lib PUBLIC glm::glm)
target_compile_definitions(supertux2_lib PUBLIC -DGLM_ENABLE_EXPERIMENTAL)

if(WIN32)
  add_executable(supertux2 WIN32 src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc)
  target_link_libraries(supertux2 ${SDL2MAIN_LIBRARIES})
else()
  add_executable(supertux2 src/main.cpp)
endif(WIN32)
target_link_libraries(supertux2 supertux2_lib)

set_target_properties(supertux2_lib PROPERTIES OUTPUT_NAME supertux2_lib)
set_target_properties(supertux2_lib PROPERTIES COMPILE_FLAGS "${SUPERTUX2_EXTRA_WARNING_FLAGS}")
if(WIN32 AND NOT VCPKG_BUILD)
  ## Copy dlls on windows
  add_custom_command(TARGET supertux2_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${DEPENDENCY_FOLDER}/dll"
    $<TARGET_FILE_DIR:supertux2_lib>)
endif(WIN32 AND NOT VCPKG_BUILD)

## Link supertux binary with squirrel and other libraries

target_include_directories(supertux2_lib SYSTEM PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(supertux2_lib SYSTEM PUBLIC ${SDL2IMAGE_INCLUDE_DIRS})
if(VCPKG_BUILD AND NOT WIN32)
  target_link_libraries(supertux2_lib PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2::SDL2_image)
else()
  target_link_libraries(supertux2_lib PUBLIC ${SDL2_LIBRARIES})
  target_link_libraries(supertux2_lib PUBLIC ${SDL2IMAGE_LIBRARIES})
endif()
target_link_libraries(supertux2_lib PUBLIC ${SDL2TTF_TARGET})

target_link_libraries(supertux2_lib PUBLIC simplesquirrel)
target_link_libraries(supertux2_lib PUBLIC sexp)
if(ENABLE_OPENAL)
  if(VCPKG_BUILD)
    target_link_libraries(supertux2_lib PUBLIC OpenAL::OpenAL)
  else()
    target_link_libraries(supertux2_lib PUBLIC ${OPENAL_LIBRARY})
  endif()
endif()
target_link_libraries(supertux2_lib PUBLIC ${OGGVORBIS_LIBRARIES})
target_link_libraries(supertux2_lib PUBLIC ${FLAC_TARGET})
if(ENABLE_SDL_MIXER)
  target_link_libraries(supertux2_lib PUBLIC ${SDL2MIXER_TARGET})
endif()

if(HAVE_OPENGL)
  target_link_libraries(supertux2_lib PUBLIC ${OPENGL_LIBRARY})
  if(NOT ENABLE_OPENGLES2)
    if(GLBINDING_FOUND)
      target_link_libraries(supertux2_lib PUBLIC ${GLBINDING_LIBRARIES})
    elseif(HAVE_GLEW)
      target_link_libraries(supertux2_lib PUBLIC GLEW::GLEW)
    endif()
  endif()
endif(HAVE_OPENGL)

if(BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()

  # each test file is its own program, so a failure names the file that failed
  file(GLOB TEST_SUPERTUX_SOURCES tests/*_test.cpp)
  foreach(TEST_SOURCE ${TEST_SUPERTUX_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_compile_options(${TEST_NAME} PRIVATE ${WARNINGS_CXX_FLAGS})
    target_link_libraries(${TEST_NAME} supertux2_lib)
    add_test(NAME ${TEST_NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMAND ${TEST_NAME})
  endforeach()

  # squirrel's own interpreter runs its samples against output recorded before we edited the VM
  add_executable(sq tests/squirrel/sq.c)
  target_link_libraries(sq squirrel sqstdlib)
  set_target_properties(sq PROPERTIES C_STANDARD 99)

  # These samples take a problem size and fall back to 1, which computes nothing worth recording
  set(SQ_ARGS_ackermann 7)
  set(SQ_ARGS_array 2000)
  set(SQ_ARGS_fibonacci 25)
  set(SQ_ARGS_list 3)
  set(SQ_ARGS_matrix 40)
  set(SQ_ARGS_methcall 200000)

  # _SQ64 is set only on 64-bit, so a 32-bit build gets int where the recordings got long long
  if(BUILD_TESTS_M32)
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_FLAGS "-m32")
    check_cxx_source_compiles("int main(){return 0;}" HAVE_M32)
    unset(CMAKE_REQUIRED_FLAGS)
    if(NOT HAVE_M32)
      message(FATAL_ERROR "BUILD_TESTS_M32 needs a compiler that can target -m32")
    endif()

    # Built from sources, as the squirrel targets carry _SQ64 and are the wrong width to link
    add_executable(sq32 tests/squirrel/sq.c ${SQUIRREL_SOURCES} ${SQSTDLIB_SOURCES})
    target_include_directories(sq32 SYSTEM PRIVATE ${SQUIRREL_DIR}/include)
    target_include_directories(sq32 PRIVATE ${SQUIRREL_DIR}/squirrel ${SQUIRREL_DIR}/sqstdlib)
    target_compile_options(sq32 PRIVATE -m32)
    target_link_options(sq32 PRIVATE -m32)
    set_target_properties(sq32 PROPERTIES C_STANDARD 99)
  endif()

  file(GLOB SQUIRREL_SAMPLES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/samples
       ${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/samples/*.nut)
  set(SQ_TARGETS sq)
  if(BUILD_TESTS_M32)
    list(APPEND SQ_TARGETS sq32)
  endif()
  foreach(SAMPLE ${SQUIRREL_SAMPLES})
    get_filename_component(SAMPLE_NAME ${SAMPLE} NAME_WE)
    foreach(SQ_TARGET ${SQ_TARGETS})
      if(SQ_TARGET STREQUAL sq)
        set(CASE squirrel_${SAMPLE_NAME})
      else()
        set(CASE squirrel_m32_${SAMPLE_NAME})
      endif()
      add_test(NAME ${CASE}
        COMMAND ${CMAKE_COMMAND}
          -DSQ=$<TARGET_FILE:${SQ_TARGET}>
          -DSCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/samples/${SAMPLE}
          -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/expected/${SAMPLE_NAME}.txt
          -DARGS=${SQ_ARGS_${SAMPLE_NAME}}
          -DTEST_NAME=${CASE}
          -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/squirrel/run_sample.cmake)
      # These finish in well under a second, so the cap only fires on a stuck VM
      set_tests_properties(${CASE} PROPERTIES TIMEOUT 30)
    endforeach()
  endforeach()
endif()

## Install stuff

if(WIN32 AND NOT UNIX)

  set(INSTALL_SUBDIR_BIN "bin" CACHE STRING "Installation subdir for binaries")
  set(INSTALL_SUBDIR_SHARE "data" CACHE STRING "Installation subdir for data")
  set(INSTALL_SUBDIR_DOC "doc" CACHE STRING "Installation subdir for docs")

  if(VCPKG_BUILD)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/\${CMAKE_INSTALL_CONFIG_NAME}/" DESTINATION ${INSTALL_SUBDIR_BIN} FILES_MATCHING PATTERN "*.dll")
  else()
    install(FILES ${DLLS} DESTINATION ${INSTALL_SUBDIR_BIN})
  endif()

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.ico DESTINATION ".")

  option(PACKAGE_VCREDIST "Package the VCREDIST libraries with the program" OFF)

  if(PACKAGE_VCREDIST)
    set(CMAKE_INSTALL_UCRT_LIBRARIES true)
    include(InstallRequiredSystemLibraries)
  endif()

else(WIN32 AND NOT UNIX)
  set(INSTALL_SUBDIR_BIN "games" CACHE STRING "Installation subdir for binaries")
  set(INSTALL_SUBDIR_SHARE "share/games/supertux2" CACHE STRING "Installation subdir for data")
  set(INSTALL_SUBDIR_DOC "share/doc/supertux2" CACHE STRING "Installation subdir for docs")

  set(LINUX_DESKTOP_ICON "supertux2")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/supertux2.desktop.in "supertux2.desktop")
  install(FILES ${CMAKE_BINARY_DIR}/supertux2.desktop DESTINATION "share/applications")
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.xpm DESTINATION "share/pixmaps/")

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/supertux2.svg DESTINATION "share/icons/hicolor/scalable/apps")
endif(WIN32 AND NOT UNIX)

install(TARGETS supertux2 DESTINATION ${INSTALL_SUBDIR_BIN})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/INSTALL.md ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${INSTALL_SUBDIR_DOC})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/credits.txt DESTINATION ${INSTALL_SUBDIR_SHARE})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/supertux2.appdata.xml DESTINATION "share/metainfo" )

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/data/images
  ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts
  ${CMAKE_CURRENT_SOURCE_DIR}/data/music
  ${CMAKE_CURRENT_SOURCE_DIR}/data/particles
  ${CMAKE_CURRENT_SOURCE_DIR}/data/scripts
  ${CMAKE_CURRENT_SOURCE_DIR}/data/shader
  ${CMAKE_CURRENT_SOURCE_DIR}/data/speech
  ${CMAKE_CURRENT_SOURCE_DIR}/data/sounds
  DESTINATION ${INSTALL_SUBDIR_SHARE})

if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
    DESTINATION ${INSTALL_SUBDIR_SHARE}
    PATTERN "data/levels/test" EXCLUDE
    PATTERN "data/levels/test_old" EXCLUDE
    PATTERN "data/levels/incubator" EXCLUDE
    PATTERN "data/levels/misc/menu.stl.in" EXCLUDE)
else()
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
    DESTINATION ${INSTALL_SUBDIR_SHARE}
    PATTERN "data/levels/misc/menu.stl.in" EXCLUDE)
endif()

install(FILES "${CMAKE_BINARY_DIR}/data/levels/misc/menu.stl" DESTINATION "${INSTALL_SUBDIR_SHARE}/levels/misc")

## Create config.h now that INSTALL_SUBDIR_* have been set.

configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h )

## Configure main menu logo
if(("${SUPERTUX_VERSION_STRING}" MATCHES "^v[0-9]+\\.[0-9]+\\.[0-9]+$") OR IS_SUPERTUX_RELEASE)
  set(LOGO_FILE "logo_final.sprite")
else()
  set(LOGO_FILE "logo.sprite")
endif()
configure_file(data/levels/misc/menu.stl.in ${CMAKE_BINARY_DIR}/data/levels/misc/menu.stl )


## CPack/Installation-specific stuff

include(InstallRequiredSystemLibraries)


set(CPACK_PACKAGE_NAME "SuperTux")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Jump'n'Run Game featuring Tux")
set(CPACK_PACKAGE_VENDOR "SuperTux Devel Team")
set(CPACK_PACKAGE_CONTACT "SuperTux Devel Team <supertux-devel@lists.lethargik.org>")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;${CMAKE_BINARY_DIR};/\\\\..*")
set(CPACK_DEBIAN_PACKAGE_NAME "supertux2")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.5), libgcc1 (>= 1:4.1), libgl1-mesa-glx | libgl1, libogg0 (>= 1.1.3), libopenal0a, libsdl-image1.2 (>= 1.2.5), libsdl1.2debian (>= 1.2.10-1), libstdc++6 (>= 4.1.2), libvorbis0a (>= 1.1.2), libvorbisfile3 (>= 1.1.2)")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Classic 2D jump 'n run sidescroller with Tux\n SuperTux is a classic 2D jump 'n run sidescroller game in a similar\n style like the original SuperMario games. This release of SuperTux\n features 9 enemies, 26 playable levels, software and OpenGL rendering\n modes, configurable joystick and keyboard input, new music and\n completely redone graphics.\n .\n This is a development snapshot of SuperTux. It may suffer from\n critical bugs and has not been fully tested. \n .\n Homepage: http://supertux.lethargik.org/")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_RPM_PACKAGE_NAME "supertux2")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Classic 2D jump 'n run sidescroller with Tux\n SuperTux is a classic 2D jump 'n run sidescroller game in a similar\n style like the original SuperMario games. This release of SuperTux\n features 9 enemies, 26 playable levels, software and OpenGL rendering\n modes, configurable joystick and keyboard input, new music and\n completely redone graphics.\n .\n This is a development snapshot of SuperTux. It may suffer from\n critical bugs and has not been fully tested. \n .\n Homepage: http://supertux.lethargik.org/")
set(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games/Action/Arcade")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${SUPERTUX_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SUPERTUX_VERSION_MINOR})

if(SUPERTUX_VERSION_TWEAK)
  set(CPACK_PACKAGE_VERSION_PATCH "${SUPERTUX_VERSION_PATCH}.${SUPERTUX_VERSION_TWEAK}")
else()
  set(CPACK_PACKAGE_VERSION_PATCH ${SUPERTUX_VERSION_PATCH})
endif(SUPERTUX_VERSION_TWEAK)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SUPERTUX_VERSION_STRING}-${SUPERTUX_SYSTEM_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SUPERTUX_VERSION_STRING}-Source")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SuperTux")
set(CPACK_PACKAGE_EXECUTABLES "supertux2" "SuperTux")
if(WIN32 AND NOT UNIX)
  set(CPACK_NSIS_INSTALLED_ICON_NAME "supertux2")
  set(CPACK_NSIS_DISPLAY_NAME "SuperTux")
  set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  set(CPACK_WIX_PROGRAM_MENU_FOLDER "")
  set(CPACK_WIX_UPGRADE_GUID "93E16F4E-0A68-422A-8ADC-47BE5B9433B2")
  set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.ico")

  set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/mk/wix/banner.png")
  set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/mk/wix/dialog.png")
  set(CPACK_GENERATOR WIX)
else(WIN32 AND NOT UNIX)
endif(WIN32 AND NOT UNIX)
include(CPack)

# move some config clutter to the advanced section
mark_as_advanced(
  INSTALL_SUBDIR_BIN
  INSTALL_SUBDIR_SHARE
  INSTALL_SUBDIR_DOC
  )

mark_as_advanced(
  CMAKE_BACKWARDS_COMPATIBILITY
  CMAKE_BUILD_TYPE
  CMAKE_INSTALL_PREFIX
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
  )

mark_as_advanced(
  GLEW_INCLUDE_DIR
  GLEW_LIBRARY
  )

mark_as_advanced(
  SDL2_INCLUDE_DIRS
  SDL2_LIBRARIES
  )

mark_as_advanced(
  SDL2IMAGE_INCLUDE_DIRS
  SDL2IMAGE_LIBRARIES
  )

mark_as_advanced(
  OPENAL_INCLUDE_DIR
  OPENAL_LIBRARY
  )

mark_as_advanced(
  SDL2TTF_INCLUDE_DIRS
  SDL2TTF_LIBRARIES
  )

mark_as_advanced(
  OGG_LIBRARY
  VORBISENC_LIBRARY
  VORBISFILE_LIBRARY
  VORBIS_INCLUDE_DIR
  VORBIS_LIBRARY
  )
