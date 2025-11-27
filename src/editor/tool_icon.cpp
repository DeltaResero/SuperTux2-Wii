// src/editor/tool_icon.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include <string>

#include "editor/tool_icon.hpp"
#include "math/rect.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

ToolIcon::ToolIcon(const std::string& icon) :
  pos(0, 0),
  surfaces(),
  mode(0),
  surf_count(0)
{
  push_mode(icon);
}

ToolIcon::~ToolIcon() {

}

void
ToolIcon::push_mode(const std::string& icon) {
  auto surface = Surface::create(icon);
  surfaces.push_back(surface);
  surf_count++;
}

void
ToolIcon::draw(DrawingContext& context) {
  context.draw_surface(surfaces[mode], pos, LAYER_GUI - 9);
}

void
ToolIcon::next_mode() {
  mode++;
  if (mode >= surf_count) {
    mode = 0;
  }
}

SurfacePtr
ToolIcon::get_current_surface() const {
  return surfaces[mode];
}

// EOF
