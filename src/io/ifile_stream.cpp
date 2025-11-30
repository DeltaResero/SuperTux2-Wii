// src/physfs/ifile_stream.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "io/ifile_stream.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"

IFileStream::IFileStream(const std::string& filename) :
  std::ifstream()
{
  std::string path = FileSystem::find(filename);
  if (path.empty()) {
    // If file not found in search paths, try opening directly as fallback
    log_debug << "File not found in search paths: " << filename << ", trying direct open" << std::endl;
    this->open(filename, std::ios::binary);
  } else {
    this->open(path, std::ios::binary);
  }

  if (!this->is_open() || !this->good()) {
    log_warning << "Failed to open file: " << filename << std::endl;
  }
}

IFileStream::~IFileStream()
{
}

// EOF
