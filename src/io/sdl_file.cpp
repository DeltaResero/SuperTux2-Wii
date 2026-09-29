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

#include "io/sdl_file.hpp"

#include <stdexcept>

#include "util/file_system.hpp"

SDL_RWops* get_SDLRWops(const std::string& filename)
{
  if (filename.empty())
    throw std::runtime_error("Couldn't open file: empty filename");

  const std::string path = FileSystem::find(filename);
  if (path.empty())
    throw std::runtime_error("Couldn't open '" + filename + "': not found");

  SDL_RWops* ops = SDL_RWFromFile(path.c_str(), "rb");
  if (!ops)
    throw std::runtime_error("Couldn't open '" + path + "': " + SDL_GetError());
  return ops;
}

SDL_RWops* get_writable_SDLRWops(const std::string& filename)
{
  if (filename.empty())
    throw std::runtime_error("Couldn't open file: empty filename");

  const std::string path = FileSystem::write_path(filename);
  if (path.empty())
    throw std::runtime_error("Couldn't open '" + filename + "' for writing: no write directory set");

  SDL_RWops* ops = SDL_RWFromFile(path.c_str(), "wb");
  if (!ops)
    throw std::runtime_error("Couldn't open '" + path + "' for writing: " + SDL_GetError());
  return ops;
}

/* EOF */
