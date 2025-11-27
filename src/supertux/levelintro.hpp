// src/supertux/levelintro.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - -- LevelIntro screen
// Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVELINTRO_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVELINTRO_HPP

#include "sprite/sprite.hpp"
#include "supertux/level.hpp"
#include "supertux/screen.hpp"
#include "supertux/timer.hpp"

class DrawingContext;
class PlayerStatus;

/**
 * Screen that welcomes the player to a level
 */
class LevelIntro : public Screen
{
private:
  static Color header_color;
  static Color author_color;
  static Color stat_hdr_color;
  static Color stat_color;


public:
  LevelIntro(const Level* level, const Statistics* best_level_statistics, const PlayerStatus* player_status);
  virtual ~LevelIntro();

  void setup();
  void draw(DrawingContext& context);
  void update(float elapsed_time);

private:
  const Level* level; /**< The level of which this is the intro screen */
  const Statistics* best_level_statistics; /**< Best level statistics of the level of which is the intro screen */
  SpritePtr player_sprite; /**< Sprite representing the player */
  SpritePtr power_sprite;
  float player_sprite_py; /**< Position (y axis) for the player sprite */
  float player_sprite_vy; /**< Velocity (y axis) for the player sprite */
  Timer player_sprite_jump_timer; /**< When timer fires, the player sprite will "jump" */
  const PlayerStatus* player_status; /**The player status passed from GameSession*/
  void draw_stats_line(DrawingContext& context, int& py, const std::string& name, const std::string& stat);

private:
  LevelIntro(const LevelIntro&);
  LevelIntro& operator=(const LevelIntro&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_LEVELINTRO_HPP

// EOF
