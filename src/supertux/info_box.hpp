// src/supertux/info_box.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_INFO_BOX_HPP
#define HEADER_SUPERTUX_SUPERTUX_INFO_BOX_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "video/surface_ptr.hpp"

class DrawingContext;
class InfoBoxLine;

/** This class is displaying a box with information text inside the game */
class InfoBox
{
public:
  InfoBox(const std::string& text);
  ~InfoBox();

  void draw(DrawingContext& context);
  void scrolldown();
  void scrollup();
  void pagedown();
  void pageup();

private:
  size_t firstline;
  std::vector<std::unique_ptr<InfoBoxLine> > lines;
  std::map<std::string, Surface*> images;
  SurfacePtr arrow_scrollup;
  SurfacePtr arrow_scrolldown;

private:
  InfoBox(const InfoBox&);
  InfoBox& operator=(const InfoBox&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_INFO_BOX_HPP

// EOF
