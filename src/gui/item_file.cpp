// src/gui/item_file.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/item_file.hpp"

#include <stdio.h>

#include "gui/menu.hpp"
#include "gui/menu_filesystem.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/menu_storage.hpp"

ItemFile::ItemFile(const std::string& text_, std::string* filename_,
                     const std::vector<std::string>& extensions_, int _id) :
  MenuItem(text_, _id),
  filename(filename_),
  extensions(extensions_)
{
}

void
ItemFile::process_action(MenuAction action) {
  if (action == MENU_ACTION_HIT) {
    MenuManager::instance().push_menu(std::unique_ptr<Menu>(new FileSystemMenu(filename, extensions)));
  }
}

// EOF
