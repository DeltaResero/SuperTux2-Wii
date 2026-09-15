// src/supertux/menu/refresh_rate_menu.cpp
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

#include "supertux/menu/refresh_rate_menu.hpp"

#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/resolution_menu.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

#include <sstream>

RefreshRateMenu::RefreshRateMenu() :
  m_refresh_rates()
{
  add_label("Refresh Rate");
  add_hl();

  const std::string in_use = current_refresh_rate_text();

  auto add_rate = [this, &in_use](int rate, const std::string& text) {
    const int id = static_cast<int>(m_refresh_rates.size());
    m_refresh_rates.push_back(rate);
    add_entry(id, text == in_use ? "[" + text + "]" : text);
  };

  add_rate(0, "Auto");

  /* No timing is asked for while the screen keeps its own mode. */
  if (g_config->fullscreen_size != Size(0, 0))
  {
    for (int rate : refresh_rates_for(g_config->fullscreen_size))
    {
      if (rate == 0) continue;

      std::ostringstream out;
      out << rate << " Hz";
      add_rate(rate, out.str());
    }
  }

  add_hl();
  add_back("Back");
}

void
RefreshRateMenu::menu_action(MenuItem* item)
{
  const size_t index = static_cast<size_t>(item->id);
  if (index >= m_refresh_rates.size())
  {
    return;
  }

  g_config->fullscreen_refresh_rate = m_refresh_rates[index];

  VideoSystem::current()->get_renderer().apply_config();
  MenuManager::instance().on_window_resize();

  /* The row that leads here names the rate in use. */
  MenuManager::instance().refresh();
  MenuManager::instance().pop_menu();
}

/* EOF */
