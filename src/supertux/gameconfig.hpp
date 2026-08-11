//  SuperTux=
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

#include "control/joystick_config.hpp"
#include "control/keyboard_config.hpp"
#include "math/size.hpp"
#include "math/vector.hpp"
#include "video/video_system.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/format.hpp>

class Config
{
public:
  Config();
  ~Config();

  void load();
  void save();

  int profile;

  /** the width/height to be used to display the game in fullscreen */
  Size fullscreen_size;

  /** refresh rate for use in fullscreen, 0 for auto */
  int fullscreen_refresh_rate;

  /** the width/height of the window managers window */
  Size window_size;

  /** the aspect ratio */
  Size aspect_size;

  float magnification;

  bool use_fullscreen;
  VideoSystem::Enum video;
  bool try_vsync;
  bool show_fps;
  bool show_player_pos;
  bool sound_enabled;
  bool music_enabled;

  /** initial random seed.  0 ==> set from time() */
  int random_seed;

  /** this variable is set if supertux should start in a specific level */
  std::string start_level;
  bool enable_script_debugger;
  std::string start_demo;
  std::string record_demo;
  
  /** this variable is set if tux should spawn somewhere which isn't the "main" spawn point*/
  boost::optional<Vector> tux_spawn_pos;

  KeyboardConfig keyboard_config;
  JoystickConfig joystick_config;

  bool developer_mode;
  bool christmas_mode;
  bool transitions_enabled;

  bool is_christmas() const {
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    date today = second_clock::local_time().date();
    date saint_nicholas_day(today.year(), Dec, 6);
    return today >= saint_nicholas_day;
  }
};

#endif

/* EOF */
