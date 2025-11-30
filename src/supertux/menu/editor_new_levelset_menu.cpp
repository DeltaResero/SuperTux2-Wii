// src/supertux/menu/editor_new_levelset_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/editor_new_levelset_menu.hpp"

#include <sstream>

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/menu/contrib_levelset_menu.hpp"
#include "supertux/menu/editor_levelset_select_menu.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/title_screen.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"

EditorNewLevelsetMenu::EditorNewLevelsetMenu() :
  levelset_name(),
  levelset_desc()
{
  add_label("New level subset");
  add_hl();

  add_textfield("Name", &levelset_name);
  add_textfield("Description", &levelset_desc);

  add_entry(1, "OK");

  add_hl();
  add_back("Back");
}

void
EditorNewLevelsetMenu::menu_action(MenuItem* item)
{
  if (item->id > 0)
  {
    if(levelset_name.empty())
    {
      std::unique_ptr<Dialog> dialog(new Dialog);
      dialog->set_text("Please enter a name for this level subset.");
      dialog->clear_buttons();
      dialog->add_button("OK", [] {});
      MenuManager::instance().set_dialog(std::move(dialog));
    }
    else
    {
      std::unique_ptr<World> new_world = World::create(levelset_name, levelset_desc);
      new_world->save();
      Editor::current()->world = move(new_world);

      MenuManager::instance().pop_menu();
      MenuManager::instance().push_menu(MenuStorage::EDITOR_LEVEL_SELECT_MENU);
    }
  }
}

// EOF
