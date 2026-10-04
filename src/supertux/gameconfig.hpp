//  SuperTux
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

#ifndef HEADER_SUPERTUX_SUPERTUX_GAMECONFIG_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAMECONFIG_HPP

#include "config.h"

#include "control/joystick_config.hpp"
#include "audio/audio_device.hpp"
#include "control/keyboard_config.hpp"
#include "math/size.hpp"
#include "math/vector.hpp"
#include "video/video_system.hpp"

#include <optional>
#include <ctime>

class Config final
{
public:
  Config();

  void load();
  void save();

  int profile;

  /** the width/height to be used to display the game in fullscreen */
  Size fullscreen_size;

  /** refresh rate for use in fullscreen, 0 for auto */
  int fullscreen_refresh_rate;

  /** the width/height of the window managers window */
  Size window_size;

  /** Window is resizable */
  bool window_resizable;

  /** the aspect ratio */
  Size aspect_size;


  float magnification;

  bool use_fullscreen;
  VideoSystem::Enum video;
  // 1 waits for every refresh, 0 never waits, -1 waits unless the frame is late
  int vsync;
  bool show_fps;
  bool show_player_pos;
  bool show_controller;
  bool sound_enabled;
  bool music_enabled;
  int sound_volume;
  int music_volume;
  /** Which sound library plays, chosen on the command line */
  AudioBackend audio_backend;

  /** initial random seed.  0 ==> set from time() */
  int random_seed;

  bool enable_script_debugger;
  std::string start_demo;
  std::string record_demo;

  /** this variable is set if tux should spawn somewhere which isn't the "main" spawn point*/
  std::optional<Vector> tux_spawn_pos;

  KeyboardConfig keyboard_config;
  JoystickConfig joystick_config;

#ifdef ENABLE_TOUCHSCREEN_SUPPORT
  bool mobile_controls;
#endif

  bool developer_mode;
  bool christmas_mode;
  bool transitions_enabled;
  bool confirmation_dialog;
  bool pause_on_focusloss;
  bool custom_mouse_cursor;

  bool is_christmas() const {
    const std::time_t now = std::time(nullptr);
    const std::tm* const today = std::localtime(&now);
    // From Saint Nicholas Day to the end of the year; tm_mon counts December as 11.
    return today != nullptr && today->tm_mon == 11 && today->tm_mday >= 6;
  }
};

#endif

/* EOF */
