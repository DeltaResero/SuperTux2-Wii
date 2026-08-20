// src/supertux/menu/options_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#ifndef HEADER_SUPERTUX_SUPERTUX_OPTIONS_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_OPTIONS_MENU_HPP

#include "gui/menu.hpp"

class ItemStringSelect;

class OptionsMenu : public Menu
{
  public:
    OptionsMenu(bool complete);
    virtual ~OptionsMenu();

    void menu_action(MenuItem* item) override;
    void on_window_resize() override;

  private:
    /** Points the one resolution row at whichever of the two settings is the
        one in use, since only the mode the game is in can be changed. */
    void update_resolution_item();

  private:
    int next_magnification;
    int next_aspect_ratio;
    int next_window_resolution;
    int next_fullscreen_resolution;
    int next_vsync;

    std::vector<std::string> magnifications;
    std::vector<std::string> aspect_ratios;
    std::vector<std::string> window_resolutions;
    std::vector<std::string> fullscreen_resolutions;
    std::vector<std::string> vsyncs;

    ItemStringSelect* resolution_item;
};

#endif

/* EOF */
