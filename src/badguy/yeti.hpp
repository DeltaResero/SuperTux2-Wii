// src/badguy/yeti.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Boss "Yeti"
// Copyright (C) 2005 Matthias Braun <matze@braunis.de>
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_YETI_HPP
#define HEADER_SUPERTUX_BADGUY_YETI_HPP

#include <memory>

#include "badguy/badguy.hpp"

class Yeti : public BadGuy
{
public:
  Yeti(const ReaderMapping& lisp);
  ~Yeti();

  void draw(DrawingContext& context);
  void initialize();
  void active_update(float elapsed_time);
  void collision_solid(const CollisionHit& hit);
  bool collision_squished(GameObject& object);
  void kill_squished(GameObject& object);
  void kill_fall();

  bool is_flammable() const;
  std::string get_class() const {
    return "yeti";
  }
  std::string get_display_name() const {
    return _("Yeti");
  }

private:
  void run();
  void jump_up();
  void be_angry();
  void drop_stalactite();
  void jump_down();

  void draw_hit_points(DrawingContext& context);

  void take_hit(Player& player);

private:
  enum YetiState {
    JUMP_DOWN,
    RUN,
    JUMP_UP,
    BE_ANGRY,
    SQUISHED
  };

private:
  YetiState state;
  Timer state_timer;
  Timer safe_timer;
  int stomp_count;
  int hit_points;
  SurfacePtr hud_head;

  float left_stand_x;
  float right_stand_x;
  float left_jump_x;
  float right_jump_x;

  void recalculate_pos();

  bool fixed_pos;
  std::string hud_icon;
};

#endif // HEADER_SUPERTUX_BADGUY_YETI_HPP

// EOF
