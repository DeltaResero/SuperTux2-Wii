// src/physfs/physfs_sdl.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_IO_PHYSFS_SDL_HPP
#define HEADER_SUPERTUX_IO_PHYSFS_SDL_HPP

#include <SDL.h>
#include <string>

SDL_RWops* get_physfs_SDLRWops(const std::string& filename);

#endif // HEADER_SUPERTUX_IO_PHYSFS_SDL_HPP

// EOF
