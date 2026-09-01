// src/supertux/command_line_arguments.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/command_line_arguments.hpp"

#include "util/file_system.hpp"

#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "supertux/gameconfig.hpp"
#include "supertux/main.hpp"
#include "version.h"
#include "math/vector.hpp"

CommandLineArguments::CommandLineArguments() :
  m_action(NO_ACTION),
  m_log_level(LOG_WARNING),
  datadir(),
  userdir(),
  fullscreen_size(),
  fullscreen_refresh_rate(),
  window_size(),
  aspect_size(),
  use_fullscreen(),
  video(),
  audio_backend(),
  show_fps(),
  show_player_pos(),
  sound_enabled(),
  music_enabled(),
  start_level(),
  enable_script_debugger(),
  start_demo(),
  record_demo(),
  tux_spawn_pos(),
  developer_mode(),
  christmas_mode()
{
}

CommandLineArguments::~CommandLineArguments()
{
}

void
CommandLineArguments::print_datadir() const
{
  // Print the datadir searchpath to stdout, one path per
  // line. Then exit. Intended for use by the supertux-editor.
  for (const std::string& path : FileSystem::get_search_paths())
    std::cout << path << std::endl;
}

void
CommandLineArguments::print_help(const char* arg0) const
{
  std::cerr
            << std::format("Usage: {} [OPTIONS] [LEVELFILE]", arg0) << "\n" << "\n"
            << "General Options:" << "\n"
            << "  -h, --help                   Show this help message and quit" << "\n"
            << "  -v, --version                Show SuperTux version and quit" << "\n"
            << "  --verbose                    Print verbose messages" << "\n"
            << "  --debug                      Print extra verbose messages" << "\n"
            << "  --print-datadir              Print SuperTux's primary data directory." << "\n" << "\n"
            << "Video Options:" << "\n"
            << "  -f, --fullscreen             Run in fullscreen mode" << "\n"
            << "  -w, --window                 Run in window mode" << "\n"
            << "  -g, --geometry WIDTHxHEIGHT  Run SuperTux in given resolution" << "\n"
            << "  -a, --aspect WIDTH:HEIGHT    Run SuperTux with given aspect ratio" << "\n"
            << "  -d, --default                Reset video settings to default values" << "\n"
            << "  --audio-backend BACKEND      Use BACKEND for sound: auto, openal or sdl" << "\n"
            << "  --renderer RENDERER          Use sdl, opengl, or auto to render" << "\n" << "\n"
            << "Audio Options:" << "\n"
            << "  --disable-sound              Disable sound effects" << "\n"
            << "  --disable-music              Disable music" << "\n" << "\n"
            << "Game Options:" << "\n"
            << "  --show-fps                   Display framerate in levels" << "\n"
            << "  --no-show-fps                Do not display framerate in levels" << "\n"
            << "  --show-pos                   Display player's current position" << "\n"
            << "  --no-show-pos                Do not display player's position" << "\n"
            << "  --developer                  Switch on developer feature" << "\n"
            << "  -s, --debug-scripts          Enable script debugger." << "\n"
            << "  --spawn-pos X,Y              Where in the level to spawn Tux. Only used if level is specified." << "\n" << "\n"
            << "Demo Recording Options:" << "\n"
            << "  --record-demo FILE LEVEL     Record a demo to FILE" << "\n"
            << "  --play-demo FILE LEVEL       Play a recorded demo" << "\n" << "\n"
            << "Directory Options:" << "\n"
            << "  --datadir DIR                Set the directory for the games datafiles" << "\n"
            << "  --userdir DIR                Set the directory for user data (savegames, etc.)" << "\n" << "\n"
            << "Environment variables:" << "\n"
            << "  SUPERTUX2_USER_DIR           Directory for user data (savegames, etc.)" << "\n"
            << "  SUPERTUX2_DATA_DIR           Directory for the games datafiles" << "\n"<< "\n"



            << std::flush;
}

void
CommandLineArguments::print_version() const
{
  std::cout << PACKAGE_NAME << " " << PACKAGE_VERSION << std::endl;
}

void
CommandLineArguments::parse_args(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "--version" || arg == "-v")
    {
      m_action = PRINT_VERSION;

    }
    else if (arg == "--help" || arg == "-h")
    {
      m_action = PRINT_HELP;
    }
    else if (arg == "--print-datadir")
    {
      m_action = PRINT_DATADIR;
    }
    else if (arg == "--debug")
    {
      m_log_level = LOG_DEBUG;
    }
    else if (arg == "--verbose")
    {
      if (m_log_level < LOG_INFO)
      {
        m_log_level = LOG_INFO;
      }
    }
    else if (arg == "--datadir")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a directory for --datadir");
      }
      else
      {
        datadir = argv[++i];
      }
    }
    else if (arg == "--userdir")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a directory for --userdir");
      }
      else
      {
        userdir = argv[++i];
      }
    }
    else if (arg == "--fullscreen" || arg == "-f")
    {
      use_fullscreen = true;
    }
    else if (arg == "--default" || arg == "-d")
    {
      use_fullscreen = false;

      window_size = Size(1280, 800);
      fullscreen_size = Size(1280, 800);
      fullscreen_refresh_rate = 0;
      aspect_size = Size(0, 0);  // auto detect
    }
    else if (arg == "--window" || arg == "-w")
    {
      use_fullscreen = false;
    }
    else if (arg == "--geometry" || arg == "-g")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a size (WIDTHxHEIGHT) for geometry argument");
      }
      else
      {
        int width, height;
        if (sscanf(argv[i], "%9dx%9d", &width, &height) != 2)
        {
          throw std::runtime_error("Invalid geometry spec, should be WIDTHxHEIGHT");
        }
        else
        {
          window_size     = Size(width, height);
          fullscreen_size = Size(width, height);
          fullscreen_refresh_rate = 0;
        }
      }
    }
    else if (arg == "--aspect" || arg == "-a")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a ratio (WIDTH:HEIGHT) for aspect ratio");
      }
      else
      {
        int aspect_width  = 0;
        int aspect_height = 0;
        if (strcmp(argv[i], "auto") == 0)
        {
          aspect_width  = 0;
          aspect_height = 0;
        }
        else if (sscanf(argv[i], "%9d:%9d", &aspect_width, &aspect_height) != 2)
        {
          throw std::runtime_error("Invalid aspect spec, should be WIDTH:HEIGHT or auto");
        }
        else
        {
          float aspect_ratio = static_cast<float>(aspect_width) / static_cast<float>(aspect_height);

          // use aspect ratio to calculate logical resolution
          if (aspect_ratio > 1) {
            aspect_size = Size(static_cast<int>(600 * aspect_ratio + 0.5),
                                         600);
          } else {
            aspect_size = Size(600,
                                         static_cast<int>(600 * 1/aspect_ratio + 0.5));
          }
        }
      }
    }
    else if (arg == "--renderer")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a renderer for renderer argument");
      }
      else
      {
        video = VideoSystem::get_video_system(argv[i]);
      }
    }
    else if (arg == "--audio-backend")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a backend for audio-backend argument");
      }
      else
      {
        audio_backend = audio_backend_from_string(argv[i]);
      }
    }
    else if (arg == "--show-fps")
    {
      show_fps = true;
    }
    else if (arg == "--no-show-fps")
    {
      show_fps = false;
    }
    else if (arg == "--show-pos")
    {
      show_player_pos = true;
    }
    else if (arg == "--no-show-pos")
    {
      show_player_pos = false;
    }
    else if (arg == "--developer")
    {
      developer_mode = true;
    }
    else if (arg == "--christmas")
    {
      christmas_mode = true;
    }
    else if (arg == "--no-christmas")
    {
      christmas_mode = false;
    }
    else if (arg == "--disable-sound" || arg == "--disable-sfx")
    {
      sound_enabled = false;
    }
    else if (arg == "--disable-music")
    {
      music_enabled = false;
    }
    else if (arg == "--play-demo")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a demo filename");
      }
      else
      {
        start_demo = argv[++i];
      }
    }
    else if (arg == "--record-demo")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a demo filename");
      }
      else
      {
        record_demo = argv[++i];
      }
    }
    else if (arg == "--spawn-pos")
    {
      Vector spawn_pos;

      if (!start_level)
        throw std::runtime_error("--spawn-pos can only be used when a levelfile is specified.");

      if (++i >= argc)
        throw std::runtime_error("Need to specify a spawn-pos X,Y");
      else
      {
        int x, y;
        if (sscanf(argv[i], "%9d,%9d", &x, &y) != 2)
          throw std::runtime_error("Invalid spawn-pos, should be X,Y");
        spawn_pos.x = x;
        spawn_pos.y = y;
      }

      tux_spawn_pos = spawn_pos;
    }
    else if (arg == "--debug-scripts" || arg == "-s")
    {
      enable_script_debugger = true;
    }
    else if (arg[0] != '-')
    {
      start_level = arg;
    }
    else
    {
      throw std::runtime_error(std::format("Unknown option '{}''. Use --help to see a list of options", arg));
    }
  }
}

void
CommandLineArguments::merge_into(Config& config)
{
#define merge_option(x) if (x) { config.x = *x; }

  merge_option(fullscreen_size);
  merge_option(fullscreen_refresh_rate);
  merge_option(window_size);
  merge_option(aspect_size);
  merge_option(use_fullscreen);
  merge_option(video);
  merge_option(audio_backend);
  merge_option(show_fps);
  merge_option(show_player_pos);
  merge_option(sound_enabled);
  merge_option(music_enabled);
  merge_option(start_level);
  merge_option(enable_script_debugger);
  merge_option(start_demo);
  merge_option(record_demo);
  merge_option(tux_spawn_pos);
  merge_option(developer_mode);
  merge_option(christmas_mode);

#undef merge_option
}

/* EOF */
