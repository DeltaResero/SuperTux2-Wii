// src/sprite/sprite_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
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

#ifndef HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP
#define HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "sprite/sprite_ptr.hpp"
#include "util/currenton.hpp"

class SpriteData;

class SpriteManager : public Currenton<SpriteManager>
{
private:
  /** Where to find a sprite that has already been read, so that a second
      caller shares it rather than reading the file again. The manager does
      not own what is listed here; the sprites themselves do. */
  typedef std::map<std::string, std::weak_ptr<SpriteData> > Sprites;
  Sprites sprites;

  /** The manager's own hold on everything above. Without it a sprite would
      go the instant the last thing using it went, so a stalactite that
      falls twice would read its pictures twice, in the middle of play. */
  std::vector<std::shared_ptr<SpriteData> > held;

public:
  SpriteManager();
  ~SpriteManager();

  /** loads a sprite. */
  SpritePtr create(const std::string& filename);

  /** Let go of every sprite that nothing is using any more. Meant for the
      moment a screen has closed and taken its objects with it, which is the
      only point at which a wait is acceptable. */
  void release_unused();

private:
  std::shared_ptr<SpriteData> load(const std::string& filename);
};

#endif

/* EOF */
