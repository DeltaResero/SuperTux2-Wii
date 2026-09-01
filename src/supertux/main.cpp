// src/supertux/main.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include "supertux/main.hpp"

#include <config.h>
#include <version.h>

#include <SDL_image.h>
#include <array>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <exception>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "audio/sound_manager.hpp"
#include "control/input_manager.hpp"
#include "gui/menu_manager.hpp"
#include "math/random_generator.hpp"
#include "object/player.hpp"
#include "io/ifile_stream.hpp"
#include "io/sdl_file.hpp"
#include "scripting/squirrel_util.hpp"
#include "scripting/scripting.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/command_line_arguments.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/game_session.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/player_status.hpp"
#include "supertux/resources.hpp"
#include "supertux/savegame.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/title_screen.hpp"
#include "supertux/sector.hpp"
#include "util/file_system.hpp"
#include "video/drawing_context.hpp"
#include "video/lightmap.hpp"
#include "video/renderer.hpp"
#include "worldmap/worldmap.hpp"

class ConfigSubsystem
{
public:
  ConfigSubsystem()
  {
    g_config.reset(new Config);
    try {
      g_config->load();
    }
    catch(const std::exception& e)
    {
      log_info << "Couldn't load config file: " << e.what() << ", using default settings" << std::endl;
    }

    // init random number stuff
    g_config->random_seed = gameRandom.srand(g_config->random_seed);
    graphicsRandom.srand(0);
    //const char *how = config->random_seed? ", user fixed.": ", from time().";
    //log_info << "Using random seed " << config->random_seed << how << std::endl;
  }

  ~ConfigSubsystem()
  {
    if (g_config)
    {
      g_config->save();
    }
    g_config.reset();
  }
};

class FileSystemSubsystem
{
private:
  std::optional<std::string> m_forced_datadir;
  std::optional<std::string> m_forced_userdir;

public:
  FileSystemSubsystem(const char* argv0,
                  std::optional<std::string> forced_datadir,
                  std::optional<std::string> forced_userdir) :
    m_forced_datadir(forced_datadir),
    m_forced_userdir(forced_userdir)
  {
    (void) argv0;
    FileSystem::clear_search_paths();

    find_userdir();
    find_datadir();
  }

  void find_datadir()
  {
    std::string datadir;
    if (m_forced_datadir)
    {
      datadir = *m_forced_datadir;
    }
    else if (const char* env_datadir = getenv("SUPERTUX2_DATA_DIR"))
    {
      datadir = env_datadir;
    }
    else
    {
      // check if we run from source dir
      char* basepath_c = SDL_GetBasePath();
      std::string basepath = basepath_c ? basepath_c : "./";
      SDL_free(basepath_c);

      if (FileSystem::exists(FileSystem::join(BUILD_DATA_DIR, "credits.stxt")))
      {
        datadir = BUILD_DATA_DIR;
        // Add config dir for supplemental files
        FileSystem::add_search_path(BUILD_CONFIG_DATA_DIR);
      }
      else
      {
        // if the game is not run from the source directory, try to find
        // the global install location
        datadir = basepath.substr(0, basepath.rfind(INSTALL_SUBDIR_BIN));
        datadir = FileSystem::join(datadir, INSTALL_SUBDIR_SHARE);
      }
    }

    if (!FileSystem::is_directory(datadir))
    {
      log_warning << "Couldn't add '" << datadir << "' to the search path: not a directory" << std::endl;
    }
    else
    {
      FileSystem::add_search_path(datadir);
    }
  }

  void find_userdir()
  {
    std::string userdir;
    if (m_forced_userdir)
    {
      userdir = *m_forced_userdir;
    }
    else if (const char* env_userdir = getenv("SUPERTUX2_USER_DIR"))
    {
      userdir = env_userdir;
    }
    else
    {
        char* prefpath = SDL_GetPrefPath(nullptr, "supertux2-wii");
        userdir = prefpath ? prefpath : std::string();
        SDL_free(prefpath);
    }
    if (!FileSystem::is_directory(userdir))
    {
        FileSystem::mkdir(userdir);
        log_info << "Created SuperTux userdir: " << userdir << std::endl;
    }

    if (!FileSystem::is_directory(userdir))
    {
      throw std::runtime_error("Failed to use userdir directory '" + userdir + "'");
    }

    FileSystem::set_write_dir(userdir);
    // Prepended: a file in the userdir shadows the one shipped in the datadir.
    FileSystem::add_search_path(userdir, /* prepend = */ true);
  }

  void print_search_path()
  {
    log_info << "WriteDir: " << FileSystem::get_write_dir() << std::endl;
    log_info << "SearchPath:" << std::endl;
    for(const auto& path : FileSystem::get_search_paths())
    {
      log_info << "  " << path << std::endl;
    }
  }

  ~FileSystemSubsystem()
  {
    FileSystem::clear_search_paths();
  }
};

class SDLSubsystem
{
public:
  SDLSubsystem()
  {
    /* Wayland has no way for a program to hand the compositor its own icon, so
       the compositor looks this name up among the installed desktop entries and
       takes the icon from there. Without it SDL falls back to the name of the
       executable, which would find an upstream SuperTux entry on a machine that
       has one. Set before SDL_Init, which is when it is read. */
    SDL_SetHint("SDL_APP_ID", "supertux2-wii");

    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
    {
      std::stringstream msg;
      msg << "Couldn't initialize SDL: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
    // just to be sure
    atexit(SDL_Quit);
  }

  ~SDLSubsystem()
  {
    SDL_Quit();
  }
};

void
Main::init_video()
{
  SDL_SetWindowTitle(VideoSystem::current()->get_renderer().get_window(), PACKAGE_NAME " " PACKAGE_VERSION);

  const char* icon_fname = "images/engine/icons/supertux-256x256.png";
  SDL_Surface* icon = IMG_Load_RW(sdl_rwops_from_file(icon_fname), true);
  if (!icon)
  {
    log_warning << "Couldn't load icon '" << icon_fname << "': " << SDL_GetError() << std::endl;
  }
  else
  {
    SDL_SetWindowIcon(VideoSystem::current()->get_renderer().get_window(), icon);
    SDL_FreeSurface(icon);
  }
  SDL_ShowCursor(0);

  log_info << (g_config->use_fullscreen?"fullscreen ":"window ")
           << " Window: "     << g_config->window_size
           << " Fullscreen: " << g_config->fullscreen_size << "@" << g_config->fullscreen_refresh_rate
           << " Area: "       << g_config->aspect_size << std::endl;
}

static Uint32 last_timelog_ticks = 0;
static const char* last_timelog_component = 0;

static inline void timelog(const char* component)
{
  Uint32 current_ticks = SDL_GetTicks();

  if(last_timelog_component != 0) {
    log_info << "Component '" << last_timelog_component <<  "' finished after " << (current_ticks - last_timelog_ticks) / 1000.0 << " seconds" << std::endl;
  }

  last_timelog_ticks = current_ticks;
  last_timelog_component = component;
}

void
Main::launch_game()
{

  SDLSubsystem sdl_subsystem;
  ConsoleBuffer console_buffer;

  timelog("controller");
  InputManager input_manager(g_config->keyboard_config, g_config->joystick_config);

  timelog("commandline");

  timelog("video");
  std::unique_ptr<VideoSystem> video_system = VideoSystem::create(g_config->video);
  DrawingContext context(*video_system);
  init_video();

  timelog("audio");
  SoundManager sound_manager;
  sound_manager.enable_sound(g_config->sound_enabled);
  sound_manager.enable_music(g_config->music_enabled);

#ifdef ENABLE_CONSOLE
  Console console(console_buffer);
#endif

  timelog("scripting");
  scripting::Scripting scripting(g_config->enable_script_debugger);

  timelog("resources");
  TileManager tile_manager;
  SpriteManager sprite_manager;
  Resources resources;

  timelog(0);

  const std::unique_ptr<Savegame> default_savegame(new Savegame(std::string()));

  GameManager game_manager;
  ScreenManager screen_manager;

  if(!g_config->start_level.empty()) {
    // A real path on the command line, not a search-path-relative name, so
    // put its directory on the search path and open it by basename.
    std::string dir = FileSystem::dirname(g_config->start_level);
    std::string filename = FileSystem::basename(g_config->start_level);
    std::string fileProtocol = "file://";
    std::string::size_type position = dir.find(fileProtocol);
    if(position != std::string::npos) {
      dir = dir.replace(position, fileProtocol.length(), "");
    }
    log_debug << "Adding dir: " << dir << std::endl;
    FileSystem::add_search_path(dir);

    if(g_config->start_level.size() > 4 &&
       g_config->start_level.compare(g_config->start_level.size() - 5, 5, ".stwm") == 0)
    {
      screen_manager.push_screen(std::unique_ptr<Screen>(
                                              new worldmap::WorldMap(filename, *default_savegame)));
    } else {
      std::unique_ptr<GameSession> session (
        new GameSession(filename, *default_savegame));

      g_config->random_seed = session->get_demo_random_seed(g_config->start_demo);
      g_config->random_seed = gameRandom.srand(g_config->random_seed);
      graphicsRandom.srand(0);

      if (g_config->tux_spawn_pos)
      {
        session->get_current_sector()->player->set_pos(*g_config->tux_spawn_pos);
      }

      if(!g_config->start_demo.empty())
        session->play_demo(g_config->start_demo);

      if(!g_config->record_demo.empty())
        session->record_demo(g_config->record_demo);
      screen_manager.push_screen(std::move(session));
    }
  } else {
    screen_manager.push_screen(std::unique_ptr<Screen>(new TitleScreen(*default_savegame)));
  }

  screen_manager.run(context);
}

int
Main::run(int argc, char** argv)
{
#ifdef WIN32
  std::string prefpath = SDL_GetPrefPath(nullptr, "supertux2-wii");
  freopen((prefpath + "/console.out").c_str(), "a", stdout);
  freopen((prefpath + "/console.err").c_str(), "a", stderr);
#endif

  int result = 0;

  try
  {
    CommandLineArguments args;

    try
    {
      args.parse_args(argc, argv);
      g_log_level = args.get_log_level();
    }
    catch(const std::exception& err)
    {
      std::cout << "Error: " << err.what() << std::endl;
      return EXIT_FAILURE;
    }

    FileSystemSubsystem filesystem_subsystem(argv[0], args.datadir, args.userdir);
    filesystem_subsystem.print_search_path();

    timelog("config");
    ConfigSubsystem config_subsystem;
    args.merge_into(*g_config);


    switch (args.get_action())
    {
      case CommandLineArguments::PRINT_VERSION:
        args.print_version();
        return 0;

      case CommandLineArguments::PRINT_HELP:
        args.print_help(argv[0]);
        return 0;

      case CommandLineArguments::PRINT_DATADIR:
        args.print_datadir();
        return 0;

      default:
        launch_game();
        break;
    }
  }
  catch(const std::exception& e)
  {
    log_fatal << "Unexpected exception: " << e.what() << std::endl;
    result = 1;
  }
  catch(...)
  {
    log_fatal << "Unexpected exception" << std::endl;
    result = 1;
  }

  return result;
}

/* EOF */
