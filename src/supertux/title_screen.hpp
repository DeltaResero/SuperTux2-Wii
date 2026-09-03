// src/supertux/title_screen.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_TITLE_HPP
#define HEADER_SUPERTUX_SUPERTUX_TITLE_HPP

#include <memory>
#include <string>

#include "math/vector.hpp"
#include "supertux/game_session.hpp"
#include "supertux/savegame.hpp"

class Block;
class CodeController;
class Player;
class Savegame;
class Sector;

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

  /** The next value out of the sequence his choices are spaced by. */
  float next_spacing();

  /** Where the top of his head would be t seconds into a jump taken now. */
  static Vector arc_head(const Rectf& body, float crown, float gravity,
                         float t);

  /** The block a jump taken now would land his head on, if there is one. */
  const Block* block_in_arc(const Sector& sector, const Player& tux) const;

  /** Whether this one is worth leaving the ground for. */
  bool worth_jumping_for(const Block& block);

  /** Put Tux where a lap begins: off the left of the view, and the given
      distance above the ground he will walk in along. */
  static void place_at_entry(Sector& sector, Player& tux, float drop);

  /** Build the level the menu is played over. */
  void create_session();

private:
  Savegame& m_savegame;
  SurfacePtr frame;
  std::unique_ptr<CodeController> controller;
  std::unique_ptr<GameSession> titlesession;
  std::string copyright_text;
  std::string wrapped_copyright; /**< copyright_text broken to fit the screen */
  int wrapped_width; /**< the width wrapped_copyright was broken for */
  bool jump_was_released; /**< whether the jump button was up last frame */
  float eagerness; /**< how much of the bonus blocks he bothers with this time */
  float crate_eagerness; /**< and how many of the crates */
  float spacing; /**< walks the range of eagerness and of the decisions */
  const Block* last_block; /**< the block the standing decision was made about */
  bool taking_block; /**< and what that decision was */

private:
  TitleScreen(const TitleScreen&);
  TitleScreen& operator=(const TitleScreen&);
};

#endif

/* EOF */
