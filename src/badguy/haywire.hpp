// src/badguy/haywire.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2010 Florian Forster <supertux at octo.it>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_HAYWIRE_HPP
#define HEADER_SUPERTUX_BADGUY_HAYWIRE_HPP

#include "audio/sound_source.hpp"
#include "badguy/walking_badguy.hpp"

class Haywire : public WalkingBadguy
{
public:
  Haywire(const ReaderMapping& reader);
  Haywire(const Vector& pos, Direction d);

  void kill_fall();
  void ignite();

  void active_update(float elapsed_time);

  bool is_freezable() const;
  void freeze();

  void stop_looping_sounds();
  void play_looping_sounds();

  std::string get_class() const {
    return "haywire";
  }
  std::string get_display_name() const {
    return _("Haywire");
  }

protected:
  bool collision_squished(GameObject& object);

private:
  void start_exploding();
  void stop_exploding();

private:
  bool is_exploding;
  float time_until_explosion;
  bool is_stunned;
  float time_stunned;

  std::unique_ptr<SoundSource> ticking;
  std::unique_ptr<SoundSource> grunting;
};

#endif // HEADER_SUPERTUX_BADGUY_HAYWIRE_HPP

// EOF
