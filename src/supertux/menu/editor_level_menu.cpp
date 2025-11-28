// src/supertux/menu/editor_level_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/editor_level_menu.hpp"

#include "gui/dialog.hpp"
#include "gui/menu.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "editor/editor.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/options_menu.hpp"
#include "supertux/level.hpp"
#include "supertux/screen_manager.hpp"
#include "util/gettext.hpp"
#include "video/color.hpp"

EditorLevelMenu::EditorLevelMenu() :
  old_tileset(Editor::current()->get_level()->tileset)
{
  bool worldmap = Editor::current()->get_worldmap_mode();
  auto level = Editor::current()->get_level();

  add_label(worldmap ? "Worldmap properties" : "Level properties");
  add_hl();
  add_textfield("Name", &(level->name));
  add_textfield("Author", &(level->author));
  add_textfield("Contact", &(level->contact));
  add_textfield("License", &(level->license));
  add_file("Tile set", &(level->tileset), std::vector<std::string>(1, ".strf"));

  if (!worldmap) {
    add_script("On menukey script", &(level->on_menukey_script));
    add_numfield("Target time", &(level->target_time));
  }

  add_hl();
  add_back("OK");
}

EditorLevelMenu::~EditorLevelMenu()
{
  auto editor = Editor::current();
  if (editor->get_level()->tileset != old_tileset) {
    editor->change_tileset();
  }
}

void
EditorLevelMenu::menu_action(MenuItem* item)
{
}

bool
EditorLevelMenu::on_back_action()
{
  auto level = Editor::current()->get_level();
  if(!level->name.empty() && !level->author.empty() && !level->license.empty())
  {
    return true;
  }
  std::unique_ptr<Dialog> dialog(new Dialog);
  if(level->name.empty())
  {
    dialog->set_text("Please enter a name for this level.");
  }
  else if(level->author.empty())
  {
    dialog->set_text("Please enter a level author for this level.");
  }
  else if(level->license.empty())
  {
    dialog->set_text("Please enter a license for this level.");
  }
  dialog->clear_buttons();
  dialog->add_button("OK", [] {});
  MenuManager::instance().set_dialog(std::move(dialog));
  return false;
}

// EOF
