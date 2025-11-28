// src/supertux/menu/editor_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/editor_menu.hpp"

#include "gui/menu.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "editor/editor.hpp"
#include "editor/input_center.hpp"
#include "editor/scroller.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/options_menu.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/world.hpp"
#include "util/gettext.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

EditorMenu::EditorMenu()
{
  bool worldmap = Editor::current()->get_worldmap_mode();
  bool is_world = Editor::current()->get_world();
  std::vector<std::string> snap_grid_sizes;
  snap_grid_sizes.push_back("1/8 tile (4px)");
  snap_grid_sizes.push_back("1/4 tile (8px)");
  snap_grid_sizes.push_back("1/2 tile (16px)");
  snap_grid_sizes.push_back("1 tile (32px)");

  add_label("Level Editor");
  add_hl();
  add_entry(MNID_RETURNTOEDITOR, "Return to editor");
  add_entry(MNID_SAVELEVEL, worldmap ? "Save current worldmap" : "Save current level");

  if (!worldmap) {
    add_entry(MNID_TESTLEVEL, "Test the level");
  }

  if (is_world) {
    add_entry(MNID_LEVELSEL, "Edit another level");
  }

  add_entry(MNID_LEVELSETSEL, "Choose another level subset");

  add_string_select(-1, "Grid size", &EditorInputCenter::selected_snap_grid_size, snap_grid_sizes);

  add_toggle(-1, "Snap objects to grid (F7)", &EditorInputCenter::snap_to_grid);
  add_toggle(-1, "Show grid (F8)", &EditorInputCenter::render_grid);
  add_toggle(-1, "Show scroller (F9)", &EditorScroller::rendered);

  add_submenu(worldmap ? "Worldmap properties" : "Level properties",
              MenuStorage::EDITOR_LEVEL_MENU);

  add_hl();
  add_entry(MNID_QUITEDITOR, "Exit level editor");
}

EditorMenu::~EditorMenu()
{
  Editor::current()->reactivate_request = true;
}

void
EditorMenu::menu_action(MenuItem* item)
{
  switch (item->id)
  {
    case MNID_RETURNTOEDITOR:
      MenuManager::instance().clear_menu_stack();
      break;

    case MNID_SAVELEVEL:
      MenuManager::instance().clear_menu_stack();
      Editor::current()->save_request = true;
      break;

    case MNID_TESTLEVEL:
      MenuManager::instance().clear_menu_stack();
      Editor::current()->test_request = true;
      break;

    case MNID_LEVELSEL:
      MenuManager::instance().set_menu(MenuStorage::EDITOR_LEVEL_SELECT_MENU);
      break;

    case MNID_LEVELSETSEL:
      MenuManager::instance().set_menu(MenuStorage::EDITOR_LEVELSET_SELECT_MENU);
      break;

    case MNID_QUITEDITOR:
      MenuManager::instance().clear_menu_stack();
      Editor::current()->quit_request = true;
      break;

    default:
      break;
  }
}

// EOF
