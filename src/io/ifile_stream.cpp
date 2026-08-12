// src/io/ifile_stream.cpp
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

#include "io/ifile_stream.hpp"

#include <stdexcept>

#include "util/file_system.hpp"

IFileStream::IFileStream(const std::string& filename) :
  std::ifstream()
{
  if (filename.empty()) {
    throw std::runtime_error("Couldn't open file: empty filename");
  }

  const std::string path = FileSystem::find(filename);
  if (path.empty()) {
    throw std::runtime_error("Couldn't open file '" + filename + "': not found");
  }

  open(path, std::ios::in | std::ios::binary);
  if (!is_open()) {
    throw std::runtime_error("Couldn't open file '" + path + "'");
  }
}

IFileStream::~IFileStream()
{
}

/* EOF */
