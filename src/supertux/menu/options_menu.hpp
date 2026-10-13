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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_OPTIONS_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_OPTIONS_MENU_HPP

#include "gui/menu.hpp"

class OptionsMenu final : public Menu
{
public:
  /** The list of sections, or one section's settings */
  enum class Section
  {
    SECTIONS,
    VIDEO,
    AUDIO,
    CONTROLS,
    EXTRAS,
    ADVANCED
  };

public:
  /** Leaves out what can't change in the middle of a game unless complete */
  OptionsMenu(Section section, bool complete);
  ~OptionsMenu() override;

  void menu_action(MenuItem& item) override;
  void on_window_resize() override;
  void refresh() override;

private:
  int next_magnification;
  int next_aspect_ratio;
  int next_vsync;
  int next_sound_volume;
  int next_music_volume;

  std::vector<std::string> magnifications;
  std::vector<std::string> aspect_ratios;
  std::vector<std::string> vsyncs;
  std::vector<std::string> sound_volumes;
  std::vector<std::string> music_volumes;

  /** Names the size in use, which can change from outside this menu */
  MenuItem* m_resolution_item;

private:
  OptionsMenu(const OptionsMenu&) = delete;
  OptionsMenu& operator=(const OptionsMenu&) = delete;
};

#endif

/* EOF */
