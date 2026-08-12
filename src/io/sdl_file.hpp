// src/io/sdl_file.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_IO_SDL_FILE_HPP
#define HEADER_SUPERTUX_IO_SDL_FILE_HPP

#include <SDL.h>
#include <string>

/** Opens a name relative to the search path as an SDL_RWops. Throws
    std::runtime_error rather than returning null, so callers may hand the
    result straight to SDL_image with the free flag set. */
SDL_RWops* sdl_rwops_from_file(const std::string& filename);

#endif

/* EOF */
