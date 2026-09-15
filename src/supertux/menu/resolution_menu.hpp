// src/supertux/menu/resolution_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_RESOLUTION_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_RESOLUTION_MENU_HPP

#include <string>
#include <vector>

#include "gui/menu.hpp"
#include "math/size.hpp"

/** How the size in use reads, for a row that only points at this menu. */
std::string current_resolution_text();

/** How the rate in use reads; zero reads "Auto". */
std::string current_refresh_rate_text();

/** Rates the screen has at a size, nearest the desktop's first. */
std::vector<int> refresh_rates_for(const Size& size);

/** Picks the size the game is shown at. Only the size chosen is asked for,
    so moving over one costs nothing. */
class ResolutionMenu final : public Menu
{
public:
  ResolutionMenu();

  void menu_action(MenuItem* item) override;
  void refresh() override;

private:
  /** What each row stands for. Zero is the desktop's own size. */
  std::vector<Size> m_sizes;
  std::vector<int> m_refresh_rates;

  /** Which of the two lists this is, settled when the menu is built. */
  bool m_fullscreen;

private:
  ResolutionMenu(const ResolutionMenu&) = delete;
  ResolutionMenu& operator=(const ResolutionMenu&) = delete;
};

#endif

/* EOF */
