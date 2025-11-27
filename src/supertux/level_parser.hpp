// src/supertux/level_parser.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVEL_PARSER_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVEL_PARSER_HPP

#include <memory>
#include <string>

class Level;
class ReaderMapping;

class LevelParser
{
public:
  static std::unique_ptr<Level> from_file(const std::string& filename);
  static std::unique_ptr<Level> from_nothing(const std::string& basedir);
  static std::unique_ptr<Level> from_nothing_worldmap(const std::string& basedir, const std::string& name);

private:
  LevelParser(Level& level);

  void load(const std::string& filepath);
  void load_old_format(const ReaderMapping& reader);
  void create(const std::string& filepath, const std::string& levelname, bool worldmap);

private:
  Level& m_level;

private:
  LevelParser(const LevelParser&) = delete;
  LevelParser& operator=(const LevelParser&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_LEVEL_PARSER_HPP

// EOF
