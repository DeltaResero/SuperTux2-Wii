// src/sprite/sprite_data.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SPRITE_SPRITE_DATA_HPP
#define HEADER_SUPERTUX_SPRITE_SPRITE_DATA_HPP

#include <map>
#include <vector>

#include "video/surface.hpp"

class ReaderMapping;

class SpriteData
{
public:
  /** cur has to be a pointer to data in the form of ((hitbox 5 10 0 0) ...) */
  SpriteData(const ReaderMapping& cur, const std::string& basedir);
  ~SpriteData();

  const std::string& get_name() const
  {
    return name;
  }

private:
  friend class Sprite;

  struct Action
  {
    Action();
    ~Action();

    std::string name;

    /** Position correction */
    float x_offset;
    float y_offset;

    /** Hitbox width */
    float hitbox_w;

    /** Hitbox height */
    float hitbox_h;

    /** Drawing priority in queue */
    int z_order;

    /** Frames per second */
    float fps;

    std::vector<SurfacePtr> surfaces;
  };

  typedef std::map <std::string, std::unique_ptr<Action> > Actions;

  void parse_action(const ReaderMapping& lispreader, const std::string& basedir);
  /** Get an action */
  const Action* get_action(const std::string& act) const;

  Actions actions;
  std::string name;
};

#endif // HEADER_SUPERTUX_SPRITE_SPRITE_DATA_HPP

// EOF
