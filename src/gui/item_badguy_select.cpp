// src/gui/item_badguy_select.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/item_badguy_select.hpp"

#include <stdio.h>

#include "gui/menu.hpp"
#include "gui/menu_badguy_select.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/menu_storage.hpp"

ItemBadguySelect::ItemBadguySelect(const std::string& text_, std::vector<std::string>* badguys_, int _id) :
  MenuItem(text_, _id),
  badguys(badguys_)
{
}

void
ItemBadguySelect::process_action(MenuAction action) {
  if (action == MENU_ACTION_HIT) {
    MenuManager::instance().push_menu(std::unique_ptr<Menu>(new BadguySelectMenu(badguys)));
  }
}

// EOF
