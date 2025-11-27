// src/supertux/textscroller.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_TEXTSCROLLER_HPP
#define HEADER_SUPERTUX_SUPERTUX_TEXTSCROLLER_HPP

#include <map>
#include <memory>

#include "supertux/screen.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class DrawingContext;
class InfoBoxLine;

/**
 * Screen that displays intro text, extro text, etc.
 */
class TextScroller : public Screen
{
public:
  TextScroller(const std::string& file);
  virtual ~TextScroller();

  void setup();
  void draw(DrawingContext& context);
  void update(float elapsed_time);

  static Color small_color;
  static Color heading_color;
  static Color reference_color;
  static Color normal_color;

private:
  float defaultspeed;
  float speed;
  std::string music;
  SurfacePtr background;
  std::vector<std::unique_ptr<InfoBoxLine> > lines;
  float scroll;
  bool fading;

private:
  TextScroller(const TextScroller&);
  TextScroller& operator=(const TextScroller&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_TEXTSCROLLER_HPP

// EOF
