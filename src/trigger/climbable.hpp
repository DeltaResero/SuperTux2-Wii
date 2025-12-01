// src/trigger/climbable.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Climbable area
// Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_TRIGGER_CLIMBABLE_HPP
#define HEADER_SUPERTUX_TRIGGER_CLIMBABLE_HPP

#include "supertux/timer.hpp"
#include "trigger/trigger_base.hpp"

class Color;
class DrawingContext;
class Player;
class ReaderMapping;

class Climbable : public TriggerBase
{
  static Color text_color;
public:
  Climbable(const ReaderMapping& reader);
  Climbable(const Rectf& area);
  ~Climbable();
  std::string get_class() const {
    return "climbable";
  }

  void event(Player& player, EventType type);
  void update(float elapsed_time);
  void draw(DrawingContext& context);

  /**
   * returns true if the player is within bounds of the Climbable
   */
  bool may_climb(Player& player) const;

protected:
  Player* climbed_by; /**< set to player who's currently climbing us, null if nobody is */
  Timer activate_try_timer; /**< try to correct mis-alignment while this timer runs */
  std::string message;

private:
  Vector new_size;

  Climbable(const Climbable&);
  Climbable& operator=(const Climbable&);
};

#endif // HEADER_SUPERTUX_TRIGGER_CLIMBABLE_HPP

// EOF
