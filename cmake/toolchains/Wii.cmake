# cmake/toolchains/Wii.cmake
# SPDX-License-Identifier: GPL-3.0-or-later
#
#  SuperTux
#  Copyright (C) 2026 DeltaResero
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

IF(DEFINED ENV{DEVKITPRO})
  SET(DEVKITPRO "$ENV{DEVKITPRO}")
ELSE()
  SET(DEVKITPRO "/opt/devkitpro")
ENDIF()

IF(NOT EXISTS "${DEVKITPRO}/cmake/Wii.cmake")
  MESSAGE(FATAL_ERROR "No devkitPro Wii support under ${DEVKITPRO}. "
                      "Install devkitPPC and libogc, or point DEVKITPRO at them.")
ENDIF()

INCLUDE("${DEVKITPRO}/cmake/Wii.cmake")

## Defaults only, so a -D on the command line still wins.
SET(ENABLE_GLEW OFF CACHE BOOL "Use GLEW to load OpenGL extensions")
SET(ENABLE_LIGHTMAP_FBO ON CACHE BOOL "Draw the lightmap into its texture with a framebuffer object")
SET(ENABLE_NPOT_TEXTURES ON CACHE BOOL "Assume the hardware accepts non-power-of-two textures")
SET(TEXTURE_ALIGNMENT 4 CACHE STRING "Round texture sides up to a multiple of this many pixels")
SET(ENABLE_OPENAL OFF CACHE BOOL "Build the OpenAL sound backend")
SET(ENABLE_SDL_MIXER ON CACHE BOOL "Build the SDL_mixer sound backend")

# EOF
