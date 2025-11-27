// src/supertux/level.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVEL_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVEL_HPP

#include "supertux/statistics.hpp"
#include "util/currenton.hpp"

class ReaderMapping;
class Sector;

/**
 * Represents a collection of Sectors running in a single GameSession.
 *
 * Each Sector in turn contains GameObjects, e.g. Badguys and Players.
 */
class Level
{
public:
  std::string name;
  std::string author;
  std::string contact;
  std::string license;
  std::string filename;
  std::string on_menukey_script;
  std::vector<std::unique_ptr<Sector> > sectors;
  Statistics  stats;
  float       target_time;
  std::string tileset;

  friend class LevelParser;

public:
  Level();
  ~Level();

  // loads a levelfile
  //void load(const std::string& filename);

  // saves to a levelfile
  void save(const std::string& filename, bool retry = false);

  void add_sector(std::unique_ptr<Sector> sector);
  const std::string& get_name() const { return name; }
  const std::string& get_author() const { return author; }

  Sector* get_sector(const std::string& name) const;

  size_t get_sector_count() const;
  Sector* get_sector(size_t num) const;

  std::string get_tileset() const { return tileset; }

  int get_total_coins() const;
  int get_total_badguys() const;
  int get_total_secrets() const;

  static Level* current() {
    return _current;
  }

  void reactivate();

private:
  static Level* _current;

  void load_old_format(const ReaderMapping& reader);

private:
  Level(const Level&);
  Level& operator=(const Level&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_LEVEL_HPP

// EOF
