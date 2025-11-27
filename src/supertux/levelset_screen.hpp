// src/supertux/levelset_screen.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVELSET_SCREEN_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVELSET_SCREEN_HPP

#include <string>

#include "supertux/screen.hpp"
#include "util/currenton.hpp"

class Savegame;

class LevelsetScreen : public Screen,
                       public Currenton<LevelsetScreen>
{
private:
  std::string m_basedir;
  std::string m_level_filename;
  Savegame& m_savegame;
  bool m_level_started;
  bool m_solved;

public:
  LevelsetScreen(const std::string& basedir, const std::string& level_filename, Savegame& savegame);
  ~LevelsetScreen();

  void draw(DrawingContext&) override;
  void update(float elapsed_time) override;

  void setup() override;
  void leave() override;

  void finished_level(bool win);

private:
  LevelsetScreen(const LevelsetScreen&) = delete;
  LevelsetScreen& operator=(const LevelsetScreen&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_LEVELSET_SCREEN_HPP

// EOF
