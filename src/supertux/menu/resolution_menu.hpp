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

/** How the size in use reads, for the row that leads to this menu */
std::string current_resolution_text();

/** How a rate reads, where zero reads "Auto" with the rate it comes to */
std::string refresh_rate_text(int rate);

/** How the rate in use reads */
std::string current_refresh_rate_text();

/** The rates the screen offers at a size */
std::vector<int> refresh_rates_for(const Size& size);

/** Picks the size the game is shown at, applying only the one chosen */
class ResolutionMenu final : public Menu
{
public:
  ResolutionMenu();

  void menu_action(MenuItem& item) override;
  void refresh() override;

private:
  std::string size_text(const Size& size) const;

private:
  /** What each row stands for, where 0x0 is the desktop or a maximised window */
  std::vector<Size> m_sizes;

  /** Whether this is the fullscreen list or the window list, settled when the menu is built */
  bool m_fullscreen;

  /** Where the refresh rate's row is, or -1 in the window list */
  int m_refresh_rate_row;

private:
  ResolutionMenu(const ResolutionMenu&) = delete;
  ResolutionMenu& operator=(const ResolutionMenu&) = delete;
};

#endif

/* EOF */
