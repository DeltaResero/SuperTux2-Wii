// src/physfs/physfs_file_system.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "physfs/physfs_file_system.hpp"

#include "physfs/ifile_stream.hpp"

PhysFSFileSystem::PhysFSFileSystem()
{
}

std::vector<std::string>
PhysFSFileSystem::open_directory(const std::string& pathname)
{
  std::vector<std::string> files;

  char** directory = PHYSFS_enumerateFiles(pathname.c_str());
  for(char** i = directory; *i != 0; ++i)
  {
    files.push_back(*i);
  }
  PHYSFS_freeList(directory);

  return files;
}

std::unique_ptr<std::istream>
PhysFSFileSystem::open_file(const std::string& filename)
{
  return std::unique_ptr<std::istream>(new IFileStream(filename));
}

bool
PhysFSFileSystem::is_directory(const std::string& filename)
{
  PHYSFS_Stat statbuf;
  PHYSFS_stat(filename.c_str(), &statbuf);
  return statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

// EOF
