// src/physfs/physfs_file_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_PHYSFS_FILE_SYSTEM_HPP
#define HEADER_SUPERTUX_PHYSFS_FILE_SYSTEM_HPP

#include <tinygettext/file_system.hpp>

class PhysFSFileSystem : public tinygettext::FileSystem
{
public:
  PhysFSFileSystem();

  std::vector<std::string>    open_directory(const std::string& pathname);
  std::unique_ptr<std::istream> open_file(const std::string& filename);
  static bool is_directory(const std::string& filename);
};

#endif // HEADER_SUPERTUX_PHYSFS_FILE_SYSTEM_HPP

// EOF
