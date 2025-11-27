// src/supertux/info_box_line.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_INFO_BOX_LINE_HPP
#define HEADER_SUPERTUX_SUPERTUX_INFO_BOX_LINE_HPP

#include <string>
#include <vector>
#include <memory>

#include "video/color.hpp"
#include "video/font_ptr.hpp"
#include "video/surface_ptr.hpp"

class DrawingContext;
class Rectf;

/**
 * Helper class for InfoBox: Represents a line of text
 */
class InfoBoxLine
{
public:
  enum LineType { NORMAL, NORMAL_LEFT, SMALL, HEADING, REFERENCE, IMAGE};

  InfoBoxLine(char format_char, const std::string& text);
  ~InfoBoxLine();

  void draw(DrawingContext& context, const Rectf& bbox, int layer);
  float get_height() const;

  static std::vector<std::unique_ptr<InfoBoxLine> > split(const std::string& text, float width);

private:
  InfoBoxLine::LineType lineType;
  FontPtr font;
  Color color;
  std::string text;
  SurfacePtr image;

private:
  InfoBoxLine(const InfoBoxLine&);
  InfoBoxLine& operator=(const InfoBoxLine&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_INFO_BOX_LINE_HPP

// EOF
