// src/gui/item_hl.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/item_hl.hpp"

#include <stdio.h>

#include "math/vector.hpp"
#include "supertux/resources.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

ItemHorizontalLine::ItemHorizontalLine() :
  MenuItem("")
{
}

void
ItemHorizontalLine::draw(DrawingContext& context, Vector pos, int menu_width, bool active) {
  // TODO
  /* Draw a horizontal line with a little 3d effect */
  context.draw_filled_rect(Vector(pos.x, pos.y - 6),
                           Vector(menu_width, 4),
                           Color(0.6f, 0.7f, 1.0f, 1.0f), LAYER_GUI);
  context.draw_filled_rect(Vector(pos.x, pos.y - 6),
                           Vector(menu_width, 2),
                           Color(1.0f, 1.0f, 1.0f, 1.0f), LAYER_GUI);
}

int
ItemHorizontalLine::get_width() const {
  return 0;
}

// EOF
