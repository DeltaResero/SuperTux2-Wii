// src/supertux/menu/world_set_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Matthew <thebatmankiller3@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/world_set_menu.hpp"

#include "audio/sound_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/fadeout.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/contrib_menu.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/options_menu.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/textscroller.hpp"
#include "supertux/title_screen.hpp"
#include "supertux/world.hpp"
#include "util/gettext.hpp"

WorldSetMenu::WorldSetMenu()
{
  add_label("Start Game");
  add_hl();
  add_entry(WORLDSET_STORY, "Story Mode");
  add_entry(WORLDSET_CONTRIB, "Contrib Levels");
  add_hl();
  add_back("Back");
}

void WorldSetMenu::menu_action(MenuItem* item)
{
  switch(item->id)
  {
    case WORLDSET_STORY:
    {
      std::unique_ptr<World> world = World::load("levels/world1");
      GameManager::current()->start_worldmap(std::move(world));
      break;
    }

    case WORLDSET_CONTRIB:
    {
      MenuManager::instance().push_menu(MenuStorage::CONTRIB_MENU);
      break;
    }
  }
}

// EOF
