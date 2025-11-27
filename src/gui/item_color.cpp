// src/gui/item_color.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/item_color.hpp"

#include <stdio.h>

#include "gui/menu.hpp"
#include "gui/menu_color.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "video/color.hpp"

ItemColor::ItemColor(const std::string& text_, Color* color_, int _id) :
  MenuItem(text_, _id),
  color(color_)
{
}

void
ItemColor::process_action(MenuAction action) {
  if (action == MENU_ACTION_HIT) {
    MenuManager::instance().push_menu(std::unique_ptr<Menu>(new ColorMenu(color)));
  }
}

Color
ItemColor::get_color() const {
  return *color;
}

// EOF
