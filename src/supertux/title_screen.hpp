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

#ifndef HEADER_SUPERTUX_SUPERTUX_TITLE_SCREEN_HPP
#define HEADER_SUPERTUX_SUPERTUX_TITLE_SCREEN_HPP

#include "supertux/screen.hpp"
#include "video/surface_ptr.hpp"

#include <memory>
#include <string>

class Block;
class CodeController;
class GameSession;
class Player;
class Savegame;
class Sector;

/** Screen that displays the SuperTux logo, lets players start a new
    game, etc. */
class TitleScreen final : public Screen
{
public:
  TitleScreen(Savegame& savegame);
  ~TitleScreen() override;

  virtual void setup() override;
  virtual void leave() override;

  virtual void draw(Compositor& compositor) override;
  virtual void update(float dt_sec, const Controller& controller) override;
  virtual std::optional<Vector> get_player_pos() const override;

private:
  void make_tux_jump();

  /** The next of a run of numbers from 0 to 1 that never bunch up */
  float next_spacing();

  /** The bonus block or crate a jump taken now would put his head into, or null */
  const Block* block_in_arc(const Sector& sector, const Player& tux) const;

  /** Draws how keen he is on bonus blocks and on crates for the lap ahead */
  void start_lap();

  /** Pushes the view along so that Tux passes beside the main menu and can walk out of it at the level's end */
  void push_view(Sector& sector) const;

  /** Puts Tux where a lap begins: left of the view and just above the ground he walks in on */
  void place_at_entry(Sector& sector, Player& tux) const;

  /** Builds the level the menu is played over */
  void create_session();

private:
  Savegame& m_savegame;
  SurfacePtr m_frame;
  std::unique_ptr<CodeController> m_controller;
  std::unique_ptr<GameSession> m_titlesession;
  std::string m_copyright_text;
  bool m_jump_was_released;
  float m_eagerness;
  float m_crate_eagerness;
  float m_spacing;
  const Block* m_last_block;
  bool m_taking_block;
  float m_menu_width;

private:
  TitleScreen(const TitleScreen&) = delete;
  TitleScreen& operator=(const TitleScreen&) = delete;
};

#endif

/* EOF */
