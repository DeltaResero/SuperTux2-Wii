// src/worldmap/level.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmail.com>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_WORLDMAP_LEVEL_HPP
#define HEADER_SUPERTUX_WORLDMAP_LEVEL_HPP

#include <memory>
#include <string>

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/artwork_interface.hpp"
#include "supertux/game_object.hpp"
#include "supertux/statistics.hpp"
#include "video/color.hpp"
#include "video/surface.hpp"

class Sprite;

namespace worldmap {

class LevelTile : public GameObject,
                  public ArtworkInterface
{
public:
  LevelTile(const std::string& basedir, const ReaderMapping& lisp);
  virtual ~LevelTile();

  virtual void draw(DrawingContext& context);
  virtual void update(float elapsed_time);

  virtual void release_artwork();
  virtual void reacquire_artwork();

  void set_solved(bool v);
  void set_perfect(bool v);

  void update_sprite_action();

public:
  Vector pos;
  std::string title;
  bool solved;
  bool perfect;
  bool auto_play; /**< true if Tux should automatically enter this level if it's unfinished */

  SpritePtr sprite;

  /** Kept so the sprite can be fetched again */
  std::string sprite_name;

  /** Statistics for level tiles */
  Statistics statistics;
  float target_time;

  /** Script that is run when the level is successfully finished */
  std::string extro_script;

  /** The colour of the level title */
  Color title_color;

private:
  std::string basedir;

private:
  LevelTile(const LevelTile&);
  LevelTile& operator=(const LevelTile&);
};

} // namespace worldmap

#endif

/* EOF */
