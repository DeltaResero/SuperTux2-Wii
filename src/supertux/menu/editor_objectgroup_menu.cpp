// src/supertux/menu/editor_objectgroup_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/editor_objectgroup_menu.hpp"

#include <sstream>

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "editor/object_group.hpp"
#include "editor/object_input.hpp"
#include "gui/menu_item.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"

EditorObjectgroupMenu::EditorObjectgroupMenu()
{
  bool worldmap = Editor::current()->get_worldmap_mode();

  add_label("Objects");
  add_hl();

  int id = 0;
  for(auto& og : Editor::current()->tileselect.object_input->groups) {
    if (worldmap == og.for_worldmap) {
      add_entry(id, og.name);
    }
    id++;
  }

  add_hl();
  add_entry(-1, "Cancel");
}

EditorObjectgroupMenu::~EditorObjectgroupMenu()
{
  Editor::current()->reactivate_request = true;
}

void
EditorObjectgroupMenu::menu_action(MenuItem* item)
{
  if (item->id >= 0)
  {
    auto tileselect = &(Editor::current()->tileselect);
    tileselect->active_objectgroup = item->id;
    tileselect->input_type = EditorInputGui::IP_OBJECT;
    tileselect->reset_pos();
    tileselect->update_mouse_icon();
  }
  MenuManager::instance().clear_menu_stack();
}

// EOF
