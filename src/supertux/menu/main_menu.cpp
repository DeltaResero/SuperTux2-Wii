//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/menu/main_menu.hpp"

#include "audio/sound_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/level_parser.hpp"
#include "supertux/levelset.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/textscroller_screen.hpp"
#include "supertux/world.hpp"
#include "util/log.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

MainMenu::MainMenu()
{
  set_center_pos(static_cast<float>(UI_WIDTH) / 2.0f,
                 static_cast<float>(UI_HEIGHT) / 2.0f + 35.0f);

  add_entry(MNID_STARTGAME, "Start Game");
  add_submenu("Options", MenuStorage::OPTIONS_MENU);
  add_entry(MNID_CREDITS, "Credits");
#ifndef REMOVE_QUIT_BUTTON
  add_entry(MNID_QUITMAINMENU, "Quit");
#endif
}

void
MainMenu::on_window_resize()
{
  set_center_pos(static_cast<float>(UI_WIDTH) / 2.0f,
                 static_cast<float>(UI_HEIGHT) / 2.0f + 35.0f);
}

void
MainMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {

    case MNID_STARTGAME:
      // World selection menu
      MenuManager::instance().push_menu(MenuStorage::WORLDSET_MENU);
      break;

     case MNID_CREDITS:
    {
      // Credits Level
      SoundManager::current()->stop_music(0.2f);
      std::unique_ptr<World> world = World::from_directory("levels/misc");
      GameManager::current()->start_level(*world, "credits.stl");
    }
    break;

    case MNID_QUITMAINMENU:
      MenuManager::instance().clear_menu_stack();
      ScreenManager::current()->quit(std::make_unique<FadeToBlack>(FadeToBlack::FADEOUT, 0.25f));
      SoundManager::current()->stop_music(0.25);
      break;
  }
}

/* EOF */
