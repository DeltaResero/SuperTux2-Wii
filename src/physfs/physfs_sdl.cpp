// src/physfs/physfs_sdl.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "physfs/physfs_sdl.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include <stdexcept>

SDL_RWops* get_physfs_SDLRWops(const std::string& filename)
{
  std::string path = FileSystem::find(filename);
  if (path.empty()) {
    log_warning << "File not found in search paths: " << filename << std::endl;
    throw std::runtime_error("File not found: " + filename);
  }

  SDL_RWops* ops = SDL_RWFromFile(path.c_str(), "rb");
  if (!ops) {
    std::string error = SDL_GetError();
    log_warning << "Couldn't open '" << path << "': " << error << std::endl;
    throw std::runtime_error("Couldn't open '" + path + "': " + error);
  }

  return ops;
}

// EOF
