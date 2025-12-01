// src/supertux/menu/main_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/main_menu.hpp"

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
#include "supertux/menu/world_set_menu.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/textscroller.hpp"
#include "supertux/title_screen.hpp"
#include "supertux/world.hpp"
#include "util/gettext.hpp"

MainMenu::MainMenu()
{
  set_center_pos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 35);

  add_entry(MNID_STARTGAME, "Start Game");
  add_submenu("Options", MenuStorage::OPTIONS_MENU);
  add_entry(MNID_CREDITS, "Credits");
  add_entry(MNID_QUITMAINMENU, "Quit");
}

void
MainMenu::on_window_resize()
{
  set_center_pos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 35);
}

void
MainMenu::menu_action(MenuItem* item)
{
  switch (item->id)
  {

    case MNID_STARTGAME:
      // World selection menu
      MenuManager::instance().push_menu(MenuStorage::WORLDSET_MENU);
      break;

    case MNID_CREDITS:
      MenuManager::instance().clear_menu_stack();
      ScreenManager::current()->push_screen(std::unique_ptr<Screen>(new TextScroller("credits.stxt")),
                                            std::unique_ptr<ScreenFade>(new FadeOut(0.5)));
      break;

    case MNID_QUITMAINMENU:
      if (true)
      {
        // instantly exit the game
        MenuManager::instance().clear_menu_stack();
        ScreenManager::current()->quit(std::unique_ptr<ScreenFade>(new FadeOut(0.25)));
        SoundManager::current()->stop_music(0.25);
      }
      else
      {
        // confirmation dialog
        std::unique_ptr<Dialog> dialog(new Dialog);
        dialog->set_text("Do you really want to quit SuperTux?");
        dialog->add_cancel_button("Cancel");
        dialog->add_default_button("Quit SuperTux", [] {
            MenuManager::instance().clear_menu_stack();
            ScreenManager::current()->quit(std::unique_ptr<ScreenFade>(new FadeOut(0.25)));
            SoundManager::current()->stop_music(0.25);
          });
        MenuManager::instance().set_dialog(std::move(dialog));
      }
      break;
  }
}

// EOF
