// src/gui/item_inactive.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/item_inactive.hpp"

#include <stdio.h>

#include "supertux/colorscheme.hpp"
#include "video/color.hpp"

ItemInactive::ItemInactive(const std::string& text_) :
  MenuItem(text_)
{
}

Color
ItemInactive::get_color() const {
  return ColorScheme::Menu::inactive_color;
}

// EOF
