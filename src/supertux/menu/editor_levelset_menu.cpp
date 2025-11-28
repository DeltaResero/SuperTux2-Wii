// src/supertux/menu/editor_levelset_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/editor_levelset_menu.hpp"

#include <physfs.h>

#include "gui/dialog.hpp"
#include "gui/menu.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "editor/editor.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/level.hpp"
#include "supertux/level_parser.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"

EditorLevelsetMenu::EditorLevelsetMenu():
  world(Editor::current()->get_world())
{
  initialize();
}

EditorLevelsetMenu::EditorLevelsetMenu(World* world_):
  world(world_)
{
  initialize();
}

EditorLevelsetMenu::~EditorLevelsetMenu()
{
  world->save();
}

void
EditorLevelsetMenu::initialize() {
  add_label("Level subset properties");
  add_hl();
  add_textfield("Name", &(world->m_title));
  add_textfield("Description", &(world->m_description));

  std::string worldmap_file = FileSystem::join(world->get_basedir(), "worldmap.stwm");
  if (PHYSFS_exists(worldmap_file.c_str())) {
    add_toggle(-1, "Do not use worldmap", &(world->m_is_levelset));
    add_entry(MNID_EDITWORLDMAP, "Edit worldmap");
  } else {
    add_entry(MNID_NEWWORLDMAP, "Create worldmap");
  }
  add_hl();
  add_back("OK");
}

void
EditorLevelsetMenu::create_worldmap()
{
  auto editor = Editor::current();
  auto basedir = world->get_basedir();
  editor->set_worldmap_mode(true);
  auto new_worldmap = LevelParser::from_nothing_worldmap(basedir, world->m_title);
  new_worldmap->save(basedir + "/" + new_worldmap->filename);
  editor->set_level(new_worldmap->filename);
  MenuManager::instance().clear_menu_stack();

  std::unique_ptr<Dialog> dialog(new Dialog);
  dialog->set_text("Share this worldmap under license CC-BY-SA 4.0 International (advised).\n"
                 "It allows modifications and redistribution by third-parties.\n"
                 "If you don't agree with this license, change it in worldmap properties.\n"
                 "DISCLAIMER: The SuperTux authors take no responsibility for your choice of license.");
  dialog->clear_buttons();
  dialog->add_button("OK", [] {});
  MenuManager::instance().set_dialog(std::move(dialog));
}

void
EditorLevelsetMenu::menu_action(MenuItem* item)
{
  auto editor = Editor::current();
  switch (item->id) {
    case MNID_EDITWORLDMAP:
        editor->set_level("worldmap.stwm");
        editor->set_worldmap_mode(true);
        MenuManager::instance().clear_menu_stack();
      break;
    case MNID_NEWWORLDMAP:
      create_worldmap();
      break;
    default:
      break;
  }

  //Editor::current()->set_level(m_levelset->get_level_filename(item->id));
}

// EOF
