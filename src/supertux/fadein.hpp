// src/supertux/fadein.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_FADEIN_HPP
#define HEADER_SUPERTUX_SUPERTUX_FADEIN_HPP

#include "supertux/screen_fade.hpp"
#include "video/color.hpp"

/**
 * Fades a screen towards a specific color
 */
class FadeIn : public ScreenFade
{
public:
  FadeIn(float fade_time, Color dest_color = Color(0, 0, 0));
  ~FadeIn();

  void update(float elapsed_time);
  void draw(DrawingContext& context);

  /// returns true if the effect is completed
  bool done() const;

private:
  Color color;
  float fade_time;
  float accum_time;
};

#endif // HEADER_SUPERTUX_SUPERTUX_FADEIN_HPP

// EOF
