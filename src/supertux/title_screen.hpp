// src/supertux/title_screen.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_TITLE_HPP
#define HEADER_SUPERTUX_SUPERTUX_TITLE_HPP

#include "supertux/game_session.hpp"
#include "supertux/savegame.hpp"

class CodeController;
class Savegame;

/**
 * Screen that displays the SuperTux logo, lets players start a new game, etc.
 */
class TitleScreen : public Screen
{
public:
  TitleScreen(Savegame& savegame);
  virtual ~TitleScreen();

  virtual void setup();
  virtual void leave();

  virtual void draw(DrawingContext& context);

  virtual void update(float elapsed_time);

private:
  void make_tux_jump();

private:
  SurfacePtr frame;
  std::unique_ptr<CodeController> controller;
  std::unique_ptr<GameSession> titlesession;
  std::string copyright_text;

private:
  TitleScreen(const TitleScreen&);
  TitleScreen& operator=(const TitleScreen&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_TITLE_HPP

// EOF
