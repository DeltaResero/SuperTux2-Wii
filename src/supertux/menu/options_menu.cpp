// src/supertux/menu/options_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2004 Tobas Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "supertux/menu/options_menu.hpp"

#include "audio/sound_manager.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/menu/joystick_menu.hpp"
#include "supertux/menu/keyboard_menu.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/profile_menu.hpp"
#include "supertux/menu/resolution_menu.hpp"
#include "util/log.hpp"
#include "util/string_util.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdio.h>
#include <memory>
#include <vector>

enum OptionsMenuIDs {
  MNID_FULLSCREEN,
  MNID_RESOLUTION,
  MNID_MAGNIFICATION,
  MNID_ASPECTRATIO,
  MNID_VSYNC,
  MNID_SOUND,
  MNID_MUSIC,
  MNID_DEVELOPER_MODE,
  MNID_CHRISTMAS_MODE,
  MNID_TRANSITIONS
};

OptionsMenu::OptionsMenu(bool complete) :
  next_magnification(0),
  next_aspect_ratio(0),
  next_vsync(0),
  magnifications(),
  aspect_ratios(),
  vsyncs(),
  resolution_item(nullptr)
{
  add_label("Options");
  add_hl();

  /* How far in or out the view may be taken. A hundred percent is the area
     the game is drawn for, and the two ends are as far as it will go either
     way, so that no screen can be made to show much more of a level than
     another. */
  magnifications.clear();
  magnifications.push_back("90%");
  magnifications.push_back("95%");
  magnifications.push_back("100%");
  magnifications.push_back("105%");
  magnifications.push_back("110%");
  next_magnification = 2; // 100%, which is also what an unset value means
  // Gets the actual magnification:
  if (g_config->magnification != 0.0f)
  {
    std::ostringstream out;
    out << (g_config->magnification*100) << "%";
    std::string magn = out.str();
    int count = 0;
    for (const auto& magnification : magnifications)
    {
      if (magnification == magn)
      {
        next_magnification = count;
        magn.clear();
        break;
      }

      ++count;
    }
    if (!magn.empty()) //magnification not in our list but accept anyway
    {
      next_magnification = magnifications.size();
      magnifications.push_back(magn);
    }
  }

  aspect_ratios.clear();
  aspect_ratios.push_back("auto");
  aspect_ratios.push_back("5:4");
  aspect_ratios.push_back("4:3");
  aspect_ratios.push_back("16:10");
  aspect_ratios.push_back("16:9");
  aspect_ratios.push_back("1368:768");
  // Gets the actual aspect ratio:
  if (g_config->aspect_size != Size(0, 0)) //auto
  {
    std::ostringstream out;
    out << g_config->aspect_size.width << ":" << g_config->aspect_size.height;
    std::string aspect_ratio = out.str();
    int cnt_ = 0;
    for(const auto& ratio : aspect_ratios)
    {
      if(ratio == aspect_ratio)
      {
        aspect_ratio.clear();
        next_aspect_ratio = cnt_;
        break;
      }
      ++cnt_;
    }

    if (!aspect_ratio.empty())
    {
      next_aspect_ratio = aspect_ratios.size();
      aspect_ratios.push_back(aspect_ratio);
    }
  }

  vsyncs.clear();
  vsyncs.push_back("on");
  vsyncs.push_back("off");
  vsyncs.push_back("adaptive");

  /* Read back what the driver settled on rather than what was asked for,
     since the two need not agree. */
  switch (VideoSystem::current()->get_vsync())
  {
    case -1: next_vsync = 2; break;
    case 0:  next_vsync = 1; break;
    case 1:  next_vsync = 0; break;
    default:
      log_warning << "Unknown vsync mode: "
                  << VideoSystem::current()->get_vsync() << std::endl;
      next_vsync = 0;
      break;
  }

  if (complete)
  {
    // Profile changes are only possible in the main menu, since
    // elsewhere it might not always work fully
    add_submenu("Select Profile", MenuStorage::PROFILE_MENU)
      ->set_help("Select a profile to play with");
  }

  add_toggle(MNID_FULLSCREEN,"Fullscreen", &g_config->use_fullscreen)
    ->set_help("Fill the entire screen");

  resolution_item = add_submenu("Resolution: " + current_resolution_text(),
                                MenuStorage::RESOLUTION_MENU, MNID_RESOLUTION);
  resolution_item->set_help(g_config->use_fullscreen
                            ? "The resolution the screen is switched to"
                            : "The size of the window the game is drawn in");

  auto magnification = add_string_select(MNID_MAGNIFICATION, "Magnification", &next_magnification, magnifications);
  magnification->set_help("Change the magnification of the game area");

  auto aspect = add_string_select(MNID_ASPECTRATIO, "Aspect Ratio", &next_aspect_ratio, aspect_ratios);
  aspect->set_help("Adjust the aspect ratio");

  auto vsync = add_string_select(MNID_VSYNC, "VSync", &next_vsync, vsyncs);
  vsync->set_help("Wait for the screen before showing a frame, which stops it tearing. Adaptive waits unless the frame is already late");

  if (SoundManager::current()->is_audio_enabled()) {
    add_toggle(MNID_SOUND, "Sound", &g_config->sound_enabled)
      ->set_help("Disable all sound effects");
    add_toggle(MNID_MUSIC, "Music", &g_config->music_enabled)
      ->set_help("Disable all music");
  } else {
    add_inactive( "Sound (disabled)");
    add_inactive( "Music (disabled)");
  }

  add_submenu("Setup Keyboard", MenuStorage::KEYBOARD_MENU)
    ->set_help("Configure key-action mappings");

  add_submenu("Setup Joystick", MenuStorage::JOYSTICK_MENU)
    ->set_help("Configure joystick control-action mappings");

  auto enable_transitions = add_toggle(MNID_TRANSITIONS, "Enable transitions", &g_config->transitions_enabled);
  enable_transitions->set_help("Enable screen transitions and smooth menu animation");

  if (g_config->developer_mode)
  {
    add_toggle(MNID_DEVELOPER_MODE, "Developer Mode", &g_config->developer_mode);
  }

  if (g_config->is_christmas() || g_config->christmas_mode)
  {
    add_toggle(MNID_CHRISTMAS_MODE, "Christmas Mode", &g_config->christmas_mode);
  }

  add_hl();
  add_back("Back");

  place_on_screen();
}

OptionsMenu::~OptionsMenu()
{
}

void
OptionsMenu::apply_video_change()
{
  Renderer& renderer = VideoSystem::current()->get_renderer();

  /* Everything is about to be redrawn at a different scale under a pointer
     that has not moved, so the arrow being clicked would slide out from under
     it. A row never moves within its menu, only the menu moves, so where the
     pointer sat relative to this menu is what gets put back. */
  int px, py;
  SDL_GetMouseState(&px, &py);
  const Vector held = renderer.to_logical(px, py) - get_center_pos();

  /* Only when the pointer is on the menu, which is to say the change came from
     clicking one of its arrows. Anyone working the keyboard with the pointer
     left elsewhere would not thank us for moving it. */
  const bool clicking = std::fabs(held.x) < get_width()/2 &&
                        std::fabs(held.y) < get_height()/2;

  renderer.apply_config();
  MenuManager::instance().on_window_resize();

  if (clicking)
  {
    renderer.warp_pointer(get_center_pos() + held);
  }
}

void
OptionsMenu::on_window_resize()
{
  Menu::on_window_resize();

  /* Fullscreen can also be turned on and off with F11, which never reaches
     this menu's own handler, so the row is brought up to date here instead:
     every path that changes the mode ends up here. */
  refresh();
}

void
OptionsMenu::refresh()
{
  /* The size can be changed from the menu this row leads to, and turning
     fullscreen on or off swaps which setting the row stands for, so the text
     is read back from the settings rather than kept in step by hand. */
  resolution_item->text = "Resolution: " + current_resolution_text();
  resolution_item->set_help(g_config->use_fullscreen
                            ? "The resolution the screen is switched to"
                            : "The size of the window the game is drawn in");
}

void
OptionsMenu::menu_action(MenuItem* item)
{
  switch (item->id) {
    case MNID_VSYNC:
      {
        int mode = 1;
        switch (next_vsync)
        {
          case 0:  mode = 1;  break;
          case 1:  mode = 0;  break;
          case 2:  mode = -1; break;
          default: assert(!"This must not be reached"); break;
        }
        g_config->vsync = mode;
        VideoSystem::current()->set_vsync(mode);
      }
      break;

    case MNID_ASPECTRATIO:
      {
        if (aspect_ratios[static_cast<size_t>(next_aspect_ratio)] == "auto")
        {
          g_config->aspect_size = Size(0, 0); // Magic values
          apply_video_change();
        }
        else if (sscanf(aspect_ratios[static_cast<size_t>(next_aspect_ratio)].c_str(), "%d:%d",
                        &g_config->aspect_size.width, &g_config->aspect_size.height) == 2)
        {
          apply_video_change();
        }
        else
        {
          assert(!"This must not be reached");
        }
      }
      break;

    case MNID_MAGNIFICATION:
      if(sscanf(magnifications[static_cast<size_t>(next_magnification)].c_str(), "%f", &g_config->magnification) == 1)
      {
        g_config->magnification /= 100.0f;
      }
      apply_video_change();
      break;

    case MNID_FULLSCREEN:
      apply_video_change();
      break;

    case MNID_SOUND:
      SoundManager::current()->enable_sound(g_config->sound_enabled);
      break;

    case MNID_MUSIC:
      SoundManager::current()->enable_music(g_config->music_enabled);
      break;

    default:
      break;
  }

  /* Every item here changes a setting, so write the file once for all of
     them rather than leaving each to remember. Only some of them used to,
     and the rest were kept no further than the next clean shutdown. */
  g_config->save();
}

/* EOF */
