// src/object/pushbutton.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - PushButton running a script
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_PUSHBUTTON_HPP
#define HEADER_SUPERTUX_OBJECT_PUSHBUTTON_HPP

#include "object/moving_sprite.hpp"

/**
 * PushButton - jump on it to run a script
 */
class PushButton : public MovingSprite
{
public:
  PushButton(const ReaderMapping& reader);

  HitResponse collision(GameObject& other, const CollisionHit& hit);
  void update(float elapsed_time);
  std::string get_class() const {
    return "pushbutton";
  }
  std::string get_display_name() const {
    return _("Button");
  }

  virtual ObjectSettings get_settings();

private:
  enum PushButtonState {
    OFF,
    ON
  };

  std::string script;
  PushButtonState state;
};

#endif // HEADER_SUPERTUX_OBJECT_PUSHBUTTON_HPP

// EOF
