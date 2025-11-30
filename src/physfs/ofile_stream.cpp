// src/physfs/ofile_stream.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_IO_OFILE_STREAM_CPP
#define HEADER_SUPERTUX_IO_OFILE_STREAM_CPP

#include "physfs/ofile_stream.hpp"
#include "util/file_system.hpp"
#include <filesystem>

OFileStream::OFileStream(const std::string& filename) :
  std::ofstream()
{
  // Always write to user dir
  std::filesystem::path path = std::filesystem::path(FileSystem::get_user_dir()) / filename;

  if (path.has_parent_path()) {
      std::filesystem::create_directories(path.parent_path());
  }

  this->open(path.string(), std::ios::binary);
}

OFileStream::~OFileStream()
{
}

#endif // HEADER_SUPERTUX_IO_OFILE_STREAM_CPP

// EOF
