// src/supertux/command_line_arguments.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_OPTIONS_HPP
#define HEADER_SUPERTUX_SUPERTUX_OPTIONS_HPP

#include <optional>

#include "math/size.hpp"
#include "math/vector.hpp"
#include "util/log.hpp"
#include "video/video_system.hpp"

class Config;

/** Command line argument parsing */
class CommandLineArguments
{
public:
  enum Action
  {
    NO_ACTION,
    PRINT_VERSION,
    PRINT_HELP,
    PRINT_DATADIR
  };

private:
  Action m_action;
  LogLevel m_log_level;

public:
  std::optional<std::string> datadir;
  std::optional<std::string> userdir;

  std::optional<Size> fullscreen_size;
  std::optional<int> fullscreen_refresh_rate;
  std::optional<Size> window_size;
  std::optional<Size> aspect_size;


  // std::optional<float> magnification;

  std::optional<bool> use_fullscreen;
   std::optional<VideoSystem::Enum> video;
  // std::optional<bool> try_vsync;
  std::optional<bool> show_fps;
  std::optional<bool> show_player_pos;
  std::optional<bool> sound_enabled;
  std::optional<bool> music_enabled;

  // std::optional<int> random_seed;

  std::optional<std::string> start_level;
  std::optional<bool> enable_script_debugger;
  std::optional<std::string> start_demo;
  std::optional<std::string> record_demo;
  std::optional<Vector> tux_spawn_pos;

  std::optional<bool> developer_mode;

  std::optional<bool> christmas_mode;

  std::optional<std::string> edit_level;

  // std::optional<std::string> locale;

public:
  CommandLineArguments();
  ~CommandLineArguments();

  Action get_action() const { return m_action; }
  LogLevel get_log_level() const { return m_log_level; }

  void parse_args(int argc, char** argv);

  void print_help(const char* arg0) const;
  void print_version() const;
  void print_datadir() const;

  void merge_into(Config& config);

private:
  CommandLineArguments(const CommandLineArguments&) = delete;
  CommandLineArguments& operator=(const CommandLineArguments&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_OPTIONS_HPP

// EOF
