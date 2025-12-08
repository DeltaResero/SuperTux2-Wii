// src/supertux/menu/menu_storage.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/menu_storage.hpp"

#include "supertux/globals.hpp"
#include "supertux/menu/cheat_menu.hpp"
#include "supertux/menu/contrib_menu.hpp"
#include "supertux/menu/game_menu.hpp"
#include "supertux/menu/joystick_menu.hpp"
#include "supertux/menu/keyboard_menu.hpp"
#include "supertux/menu/main_menu.hpp"
#include "supertux/menu/options_menu.hpp"
#include "supertux/menu/profile_menu.hpp"
#include "supertux/menu/worldmap_menu.hpp"
#include "supertux/menu/worldmap_cheat_menu.hpp"
#include "supertux/menu/world_set_menu.hpp"

MenuStorage* MenuStorage::s_instance = 0;

MenuStorage&
MenuStorage::instance()
{
  assert(s_instance);
  return *s_instance;
}

MenuStorage::MenuStorage()
{
  assert(!s_instance);
  s_instance = this;
}

MenuStorage::~MenuStorage()
{
  s_instance = nullptr;
}

std::unique_ptr<Menu>
MenuStorage::create(MenuId menu_id)
{
  switch(menu_id)
  {
    case MAIN_MENU:
      return std::make_unique<MainMenu>();

    case OPTIONS_MENU:
      return std::make_unique<OptionsMenu>(true);

    case INGAME_OPTIONS_MENU:
      return std::make_unique<OptionsMenu>(false);

    case PROFILE_MENU:
      return std::make_unique<ProfileMenu>();

    case KEYBOARD_MENU:
      return std::make_unique<KeyboardMenu>(*InputManager::current());

    case JOYSTICK_MENU:
      return std::make_unique<JoystickMenu>(*InputManager::current());

    case WORLDMAP_MENU:
      return std::make_unique<WorldmapMenu>();

    case WORLDMAP_CHEAT_MENU:
      return std::make_unique<WorldmapCheatMenu>();

    case GAME_MENU:
      return std::make_unique<GameMenu>();

    case CHEAT_MENU:
      return std::make_unique<CheatMenu>();

    case WORLDSET_MENU:
      return std::make_unique<WorldSetMenu>();

    case CONTRIB_MENU:
      return std::make_unique<ContribMenu>();

    case CONTRIB_WORLD_MENU:
      return 0; //return new ContribWorldMenu();

    case NO_MENU:
      return std::unique_ptr<Menu>();

    default:
      assert(!"unknown MenuId provided");
      return std::unique_ptr<Menu>();
  }
}

// EOF
