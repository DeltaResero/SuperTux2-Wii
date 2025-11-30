// src/supertux/levelset.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/levelset.hpp"

#include <filesystem>
#include <algorithm>

#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/string_util.hpp"

Levelset::Levelset(const std::string& basedir, bool recursively) :
  m_basedir(basedir),
  m_levels()
{
  walk_directory(m_basedir, recursively);
  std::sort(m_levels.begin(), m_levels.end(), StringUtil::numeric_less);
}

int
Levelset::get_num_levels() const
{
  return static_cast<int>(m_levels.size());
}

std::string
Levelset::get_level_filename(int i) const
{
  return m_levels[i];
}

void
Levelset::walk_directory(const std::string& directory, bool recursively)
{
  bool is_basedir = (directory == m_basedir);
  std::string search_path = FileSystem::find(directory);

  if (search_path.empty() || !std::filesystem::is_directory(search_path)) {
    log_warning << "Couldn't read subset dir '" << directory << "'" << std::endl;
    return;
  }

  for (const auto& entry : std::filesystem::directory_iterator(search_path)) {
      std::string filename = entry.path().filename().string();
      std::string filepath = FileSystem::join(directory, filename);

      if (entry.is_directory() && recursively) {
          walk_directory(filepath, true);
      }

      if (StringUtil::has_suffix(filename, ".stl")) {
          if(is_basedir) {
            m_levels.push_back(filename);
          } else {
            // Replace basedir part of file path plus slash.
            std::string rel_path = filepath;
            rel_path = rel_path.replace(0, m_basedir.length() + 1, "");
            m_levels.push_back(rel_path);
          }
      }
  }
}

// EOF
