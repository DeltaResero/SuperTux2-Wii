// src/gui/item_script.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/item_script.hpp"

#include <stdio.h>

#include "gui/menu.hpp"
#include "gui/menu_manager.hpp"
#include "gui/menu_script.hpp"
#include "supertux/menu/menu_storage.hpp"

ItemScript::ItemScript(const std::string& text_, std::string* script_, int _id) :
  MenuItem(text_, _id),
  script(script_)
{
}

void
ItemScript::process_action(MenuAction action) {
  if (action == MENU_ACTION_HIT) {
    MenuManager::instance().push_menu(std::unique_ptr<Menu>(new ScriptMenu(script)));
  }
}

// EOF
