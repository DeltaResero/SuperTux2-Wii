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

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <sstream>

#include "audio/sound_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/item_goto.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/item_toggle.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/game_session.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/resolution_menu.hpp"
#include "util/log.hpp"
#include "video/renderer.hpp"
#include "video/viewport.hpp"


namespace {

bool less_than_volume(const std::string& lhs, const std::string& rhs) {
  int lhs_i, rhs_i;
  if (sscanf(lhs.c_str(), "%i", &lhs_i) == 1 &&
      sscanf(rhs.c_str(), "%i", &rhs_i) == 1)
  {
    return lhs_i < rhs_i;
  }

  return false;
}


} // namespace

enum OptionsMenuIDs {
  MNID_WINDOW_RESIZABLE,
  MNID_FULLSCREEN,
  MNID_MAGNIFICATION,
  MNID_ASPECTRATIO,
  MNID_VSYNC,
  MNID_SOUND,
  MNID_MUSIC,
  MNID_SOUND_VOLUME,
  MNID_MUSIC_VOLUME,
  MNID_DEVELOPER_MODE,
  MNID_CHRISTMAS_MODE,
  MNID_TRANSITIONS,
  MNID_CONFIRMATION_DIALOG,
  MNID_PAUSE_ON_FOCUSLOSS,
  MNID_CUSTOM_CURSOR
#ifdef ENABLE_TOUCHSCREEN_SUPPORT
  , MNID_MOBILE_CONTROLS
#endif
};

OptionsMenu::OptionsMenu(Section section, bool complete) :
  next_magnification(0),
  next_aspect_ratio(0),
  next_vsync(0),
  next_sound_volume(0),
  next_music_volume(0),
  magnifications(),
  aspect_ratios(),
  vsyncs(),
  sound_volumes(),
  music_volumes(),
  m_resolution_item(nullptr)
{
  switch (section)
  {
    case Section::SECTIONS:
      add_label("Options");
      add_hl();
      add_submenu("Video", MenuStorage::VIDEO_MENU);
      add_submenu("Audio", MenuStorage::AUDIO_MENU);
      add_submenu("Controls", MenuStorage::CONTROLS_MENU);
      add_submenu("Extras", complete ? MenuStorage::EXTRAS_MENU : MenuStorage::INGAME_EXTRAS_MENU);
      add_submenu("Advanced", MenuStorage::ADVANCED_MENU);
      add_hl();
      add_back("Back");
      return;

    case Section::VIDEO:
      add_label("Video");
      break;

    case Section::AUDIO:
      add_label("Audio");
      break;

    case Section::CONTROLS:
      add_label("Controls");
      break;

    case Section::EXTRAS:
      add_label("Extras");
      break;

    case Section::ADVANCED:
      add_label("Advanced");
      break;
  }
  add_hl();

  // 100% shows what a 1920x1080 screen showed, and every screen of the same shape shows the same
  magnifications.clear();
  const int min_percent = static_cast<int>(Viewport::s_min_zoom * 100.0f + 0.5f);
  const int max_percent = static_cast<int>(Viewport::s_max_zoom * 100.0f + 0.5f);
  for (int percent = min_percent; percent <= max_percent; percent += 5)
  {
    magnifications.push_back(std::to_string(percent) + "%");
  }
  {
    // An old config may hold a zoom outside the range, which shows as its nearest step
    const float zoom = (g_config->magnification == 0.0f) ? 1.0f : g_config->magnification;
    const int percent = std::clamp(static_cast<int>(zoom * 100.0f + 0.5f), min_percent, max_percent);
    next_magnification = (percent - min_percent + 2) / 5;
  }

  aspect_ratios.clear();
  aspect_ratios.push_back("auto");
  aspect_ratios.push_back("5:4");
  aspect_ratios.push_back("4:3");
  aspect_ratios.push_back("16:10");
  aspect_ratios.push_back("16:9");
  // Gets the actual aspect ratio:
  if (g_config->aspect_size != Size(0, 0)) //auto
  {
    std::ostringstream out;
    out << g_config->aspect_size.width << ":" << g_config->aspect_size.height;
    std::string aspect_ratio = out.str();
    int cnt_ = 0;
    for (const auto& ratio : aspect_ratios)
    {
      if (ratio == aspect_ratio)
      {
        aspect_ratio.clear();
        next_aspect_ratio = cnt_;
        break;
      }
      ++cnt_;
    }

    if (!aspect_ratio.empty())
    {
      next_aspect_ratio = static_cast<int>(aspect_ratios.size());
      aspect_ratios.push_back(aspect_ratio);
    }
  }

  { // vsync
    vsyncs.push_back("on");
    vsyncs.push_back("off");
    vsyncs.push_back("adaptive");
    int mode = VideoSystem::current()->get_vsync();

    switch (mode)
    {
      case -1:
        next_vsync = 2;
        break;

      case 0:
        next_vsync = 1;
        break;

      case 1:
        next_vsync = 0;
        break;

      default:
        log_warning << "Unknown swap mode: " << mode << std::endl;
        next_vsync = 0;
    }
  }

  // Sound Volume
  sound_volumes.clear();
  for (const char* percent : {"0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%"}) {
    sound_volumes.push_back(percent);
  }

  std::ostringstream sound_vol_stream;
  sound_vol_stream << g_config->sound_volume << "%";
  std::string sound_vol_string = sound_vol_stream.str();

  if (std::find(sound_volumes.begin(),
               sound_volumes.end(), sound_vol_string) == sound_volumes.end())
  {
    sound_volumes.push_back(sound_vol_string);
  }

  std::sort(sound_volumes.begin(), sound_volumes.end(), less_than_volume);

  std::ostringstream out;
  out << g_config->sound_volume << "%";
  std::string sound_volume = out.str();
  int cnt_ = 0;
  for (const auto& volume : sound_volumes)
  {
    if (volume == sound_volume)
    {
      sound_volume.clear();
      next_sound_volume = cnt_;
      break;
    }
    ++cnt_;
  }

  // Music Volume
  music_volumes.clear();
  for (const char* percent : {"0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%"}) {
    music_volumes.push_back(percent);
  }

  std::ostringstream music_vol_stream;
  music_vol_stream << g_config->music_volume << "%";
  std::string music_vol_string = music_vol_stream.str();

  if (std::find(music_volumes.begin(),
               music_volumes.end(), music_vol_string) == music_volumes.end())
  {
    music_volumes.push_back(music_vol_string);
  }

  std::sort(music_volumes.begin(), music_volumes.end(), less_than_volume);

  out.str("");
  out.clear();
  out << g_config->music_volume << "%";
  std::string music_volume = out.str();
  cnt_ = 0;
  for (const auto& volume : music_volumes)
  {
    if (volume == music_volume)
    {
      music_volume.clear();
      next_music_volume = cnt_;
      break;
    }
    ++cnt_;
  }

  if (section == Section::VIDEO)
  {
#ifndef ENABLE_TOUCHSCREEN_SUPPORT
    add_toggle(MNID_FULLSCREEN,"Window Resizable", &g_config->window_resizable)
      .set_help("Allow window resizing, might require a restart to take effect");

    add_toggle(MNID_FULLSCREEN,"Fullscreen", &g_config->use_fullscreen)
      .set_help("Fill the entire screen");

    m_resolution_item = &add_submenu("", MenuStorage::RESOLUTION_MENU);
#endif

    MenuItem& magnification = add_string_select(MNID_MAGNIFICATION, "Camera Zoom", &next_magnification, magnifications);
    magnification.set_help("Zoom the camera view of levels and worldmaps");

    MenuItem& vsync = add_string_select(MNID_VSYNC, "VSync", &next_vsync, vsyncs);
    vsync.set_help("Set the VSync mode");

#ifndef ENABLE_TOUCHSCREEN_SUPPORT
    MenuItem& aspect = add_string_select(MNID_ASPECTRATIO, "Aspect Ratio", &next_aspect_ratio, aspect_ratios);
    aspect.set_help("Adjust the aspect ratio");
#endif
  }

  if (section == Section::AUDIO)
  {
    if (SoundManager::current()->is_audio_enabled())
    {
      add_toggle(MNID_SOUND, "Sound", &g_config->sound_enabled)
        .set_help("Disable all sound effects");
      add_toggle(MNID_MUSIC, "Music", &g_config->music_enabled)
        .set_help("Disable all music");

      MenuItem& sound_volume_select = add_string_select(MNID_SOUND_VOLUME, "Sound Volume", &next_sound_volume, sound_volumes);
      sound_volume_select.set_help("Adjust sound volume");

      MenuItem& music_volume_select = add_string_select(MNID_MUSIC_VOLUME, "Music Volume", &next_music_volume, music_volumes);
      music_volume_select.set_help("Adjust music volume");
    }
    else
    {
      add_inactive( "Sound (disabled)");
      add_inactive( "Music (disabled)");
    }
  }

  if (section == Section::CONTROLS)
  {
    add_submenu("Setup Keyboard", MenuStorage::KEYBOARD_MENU)
      .set_help("Configure key-action mappings");

    add_submenu("Setup Joystick", MenuStorage::JOYSTICK_MENU)
      .set_help("Configure joystick control-action mappings");

#ifdef ENABLE_TOUCHSCREEN_SUPPORT
    add_toggle(MNID_MOBILE_CONTROLS, "On-screen controls", &g_config->mobile_controls)
        .set_help("Toggle on-screen controls for mobile devices");
#endif
  }

  if (section == Section::EXTRAS)
  {
    if (complete)
    {
      // Profile changes are only possible in the main menu as
      // elsewhere it might not always work fully
      add_submenu("Select Profile", MenuStorage::PROFILE_MENU)
        .set_help("Select a profile to play with");
    }

    MenuItem& enable_transitions = add_toggle(MNID_TRANSITIONS, "Enable transitions", &g_config->transitions_enabled);
    enable_transitions.set_help("Enable screen transitions and smooth menu animation");

    if (g_config->is_christmas() || g_config->christmas_mode)
    {
      add_toggle(MNID_CHRISTMAS_MODE, "Christmas Mode", &g_config->christmas_mode);
    }
  }

  if (section == Section::ADVANCED)
  {
    add_toggle(MNID_CONFIRMATION_DIALOG, "Confirmation Dialog", &g_config->confirmation_dialog).set_help("Confirm aborting level");
    add_toggle(MNID_PAUSE_ON_FOCUSLOSS, "Pause on focus loss", &g_config->pause_on_focusloss)
      .set_help("Automatically pause the game when the window loses focus");
    add_toggle(MNID_CUSTOM_CURSOR, "Use custom mouse cursor", &g_config->custom_mouse_cursor).set_help("Whether the game renders its own cursor or uses the system's cursor");

    if (g_config->developer_mode)
    {
      add_toggle(MNID_DEVELOPER_MODE, "Developer Mode", &g_config->developer_mode);
    }
  }

  add_hl();
  add_back("Back");

  refresh();
}

OptionsMenu::~OptionsMenu()
{
}

void
OptionsMenu::on_window_resize()
{
  Menu::on_window_resize();

  // F11 and dragging the window change the size without passing through this menu
  refresh();
}

void
OptionsMenu::refresh()
{
  if (m_resolution_item)
  {
    m_resolution_item->set_text("Resolution: " + current_resolution_text());
    m_resolution_item->set_help(g_config->use_fullscreen ? "The resolution the screen is switched to" :
                                                           "The size of the window the game is drawn in");
    calculate_width();
  }
}

void
OptionsMenu::menu_action(MenuItem& item)
{
  switch (item.get_id()) {
    case MNID_ASPECTRATIO:
      {
        if (aspect_ratios[next_aspect_ratio] == "auto")
        {
          g_config->aspect_size = Size(0, 0); // Magic values
          VideoSystem::current()->apply_config();
          MenuManager::instance().on_window_resize();
        }
        else if (sscanf(aspect_ratios[next_aspect_ratio].c_str(), "%d:%d",
                        &g_config->aspect_size.width, &g_config->aspect_size.height) == 2)
        {
          VideoSystem::current()->apply_config();
          MenuManager::instance().on_window_resize();
        }
        else
        {
          log_fatal << "Invalid aspect ratio " << aspect_ratios[next_aspect_ratio] << " specified" << std::endl;
          assert(false);
        }
      }
      break;

    case MNID_MAGNIFICATION:
      if (sscanf(magnifications[next_magnification].c_str(), "%f", &g_config->magnification) == 1)
      {
        g_config->magnification /= 100.0f;
      }
      VideoSystem::current()->apply_config();
      MenuManager::instance().on_window_resize();
      break;

    case MNID_VSYNC:
      switch (next_vsync)
      {
        case 2:
          g_config->vsync = -1;
          break;

        case 1:
          g_config->vsync = 0;
          break;

        case 0:
          g_config->vsync = 1;
          break;

        default:
          assert(false);
          break;
      }
      VideoSystem::current()->set_vsync(g_config->vsync);
      break;

    case MNID_FULLSCREEN:
      VideoSystem::current()->apply_config();
      MenuManager::instance().on_window_resize();
      break;

    case MNID_SOUND:
      SoundManager::current()->enable_sound(g_config->sound_enabled);
      break;

    case MNID_SOUND_VOLUME:
      if (sscanf(sound_volumes[next_sound_volume].c_str(), "%i", &g_config->sound_volume) == 1)
      {
        bool sound_enabled = g_config->sound_volume > 0 ? true : false;
        SoundManager::current()->enable_sound(sound_enabled);
        SoundManager::current()->set_sound_volume(g_config->sound_volume);
      }
      break;

    case MNID_MUSIC:
      SoundManager::current()->enable_music(g_config->music_enabled);
      break;

    case MNID_MUSIC_VOLUME:
      if (sscanf(music_volumes[next_music_volume].c_str(), "%i", &g_config->music_volume) == 1)
      {
        bool music_enabled = g_config->music_volume > 0 ? true : false;
        SoundManager::current()->enable_music(music_enabled);
        SoundManager::current()->set_music_volume(g_config->music_volume);
      }
      break;

    case MNID_CUSTOM_CURSOR:
      SDL_ShowCursor(g_config->custom_mouse_cursor ? 0 : 1);
      break;

    default:
      break;
  }

  // Every item with an id is a setting, so it reaches the file now rather than at a clean exit
  if (item.get_id() >= 0)
  {
    g_config->save();
  }
}

/* EOF */
