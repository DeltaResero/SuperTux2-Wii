// src/supertux/world.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_WORLD_HPP
#define HEADER_SUPERTUX_SUPERTUX_WORLD_HPP

#include <memory>
#include <squirrel.h>
#include <string>
#include <vector>

class World
{
private:
  World();

  void load_(const std::string& directory);
  void create_(const std::string& directory, const std::string& title, const std::string& desc);

public:
  /**
      Load a World

      @param directory  Directory containing the info file, e.g. "levels/world1"
  */
  static std::unique_ptr<World> load(const std::string& directory);
  static std::unique_ptr<World> create(const std::string& title, const std::string& desc);

public:
  ~World();

  std::string get_basedir() const;
  std::string get_title() const;

  bool hide_from_contribs() const { return m_hide_from_contribs; }

  bool is_levelset() const { return m_is_levelset; }
  bool is_worldmap() const { return !m_is_levelset; }

  std::string get_worldmap_filename() const { return m_worldmap_filename; }
  std::string get_savegame_filename() const { return m_savegame_filename; }

  void save(bool retry = false);
  void set_default_values();

private:
  std::string m_basedir;
  std::string m_worldmap_filename;
  std::string m_savegame_filename;

public:
  std::string m_title;
  std::string m_description;

private:
  bool m_hide_from_contribs;

public:
  bool m_is_levelset;

private:
  World(const World&) = delete;
  World& operator=(const World&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_WORLD_HPP

// EOF
