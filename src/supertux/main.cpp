// src/supertux/main.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/main.hpp"

#include <config.h>
#include <version.h>

#include <SDL_image.h>
#include <format>
#include <optional>
#include <array>
#include <iostream>
#include <stdio.h>

#include "audio/sound_manager.hpp"
#include "control/input_manager.hpp"
#include "gui/menu_manager.hpp"
#include "math/random_generator.hpp"
#include "object/player.hpp"
#include "io/sdl_io.hpp"
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
#include "util/gettext.hpp"
#include "video/drawing_context.hpp"
#include "video/lightmap.hpp"
#include "video/renderer.hpp"
#include "worldmap/worldmap.hpp"

// --- Wii Specific Includes & Helpers ---
#ifdef _WII_
#include <gccore.h>
#include <fat.h>
#include <wiiuse/wpad.h>
#include <unistd.h>
#include <cstdlib>

static void* xfb = nullptr;
static GXRModeObj* rmode = nullptr;

void wii_console_init()
{
  VIDEO_Init();
  rmode = VIDEO_GetPreferredMode(nullptr);
  xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

  // Clear framebuffer to black to prevent visual garbage
  VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

  // Calculate safe console dimensions to prevent buffer overflow (Overscan compensation)
  int console_x = 20;
  int console_y = 20;
  int console_w = rmode->fbWidth - (console_x * 2);
  int console_h = rmode->xfbHeight - (console_y * 2);

  console_init(xfb, console_x, console_y, console_w, console_h, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

  VIDEO_Configure(rmode);
  VIDEO_SetNextFramebuffer(xfb);
  VIDEO_SetBlack(FALSE);
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
}
#endif // _WII_

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

class SDLSubsystem
{
public:
  SDLSubsystem()
  {
#ifdef _WII_
    // Initialize Wii hardware BEFORE SDL
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
#endif

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
#ifndef _WII_ // Wii: Skip window naming and icon loading (unnecessary and causes crashes)
  SDL_SetWindowTitle(VideoSystem::current()->get_renderer().get_window(), PACKAGE_NAME " " PACKAGE_VERSION);

  const char* icon_fname = "images/engine/icons/supertux-256x256.png";
  SDL_Surface* icon = IMG_Load_RW(get_physfs_SDLRWops(icon_fname), true);
  if (!icon)
  {
    log_warning << "Couldn't load icon '" << icon_fname << "': " << SDL_GetError() << std::endl;
  }
  else
  {
    SDL_SetWindowIcon(VideoSystem::current()->get_renderer().get_window(), icon);
    SDL_FreeSurface(icon);
  }
#endif

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

#ifndef DISABLE_CONSOLE
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
    // we have a normal path specified at commandline, not a physfs path.
    // So we simply mount that path here...
    std::string dir = FileSystem::dirname(g_config->start_level);
    std::string filename = FileSystem::basename(g_config->start_level);
    std::string fileProtocol = "file://";
    std::string::size_type position = dir.find(fileProtocol);
    if(position != std::string::npos) {
      dir = dir.replace(position, fileProtocol.length(), "");
    }
    log_debug << "Adding dir: " << dir << std::endl;

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
#ifdef _WII_
  // Initialize Wii hardware FIRST
  fatInitDefault();
  wii_console_init();

  printf("\n");
  printf("==========================================\n");
  printf("  SuperTux %s - Wii Port\n", PACKAGE_VERSION);
  printf("==========================================\n");
  printf("Initializing SD card and filesystem...\n");
  printf("\n");
#endif

#ifdef WIN32
	//SDL is used instead of PHYSFS because both create the same path in app data
	//However, PHYSFS is not yet initizlized, and this should be run before anything is initialized
	std::string prefpath = SDL_GetPrefPath("SuperTux", "supertux2");
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

    FileSystem::init(argv[0]);

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

  #ifdef _WII_
  // Wii: Clean exit message
  printf("\n");
  printf("==========================================\n");
  printf("  Press HOME button to exit to loader\n");
  printf("==========================================\n");
  while(1)
  {
    WPAD_ScanPads();
    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
    VIDEO_WaitVSync();
  }

  // Return to Homebrew Channel
  SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
#endif

  return result;
}

// EOF
