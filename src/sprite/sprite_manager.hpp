// src/sprite/sprite_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP
#define HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP

#include <map>
#include <memory>
#include <string>

#include "sprite/sprite_ptr.hpp"
#include "util/currenton.hpp"

class SpriteData;

class SpriteManager : public Currenton<SpriteManager>
{
private:
  typedef std::map<std::string, std::unique_ptr<SpriteData> > Sprites;
  Sprites sprites;

public:
  SpriteManager();
  ~SpriteManager();

  /** loads a sprite. */
  SpritePtr create(const std::string& filename);

private:
  SpriteData* load(const std::string& filename);
};

#endif // HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP

// EOF
