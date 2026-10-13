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

#include "supertux/menu/resolution_menu.hpp"

#include <SDL.h>

#include <algorithm>
#include <sstream>

#include "gui/item_action.hpp"
#include "gui/item_goto.hpp"
#include "gui/item_inactive.hpp"
#include "gui/menu_item.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/menu/video_mode_dialog.hpp"

namespace {

// Fewer lines than this squeeze the menus' text past reading
const int MIN_FULLSCREEN_WIDTH = 640;
const int MIN_FULLSCREEN_HEIGHT = 480;

// The sizes a window is offered at, as far as the desktop has room for them
const Size WINDOW_SIZES[] = {
  Size(640, 480), Size(854, 480), Size(800, 600), Size(1280, 720), Size(1280, 800),
  Size(1440, 900), Size(1920, 1080), Size(1920, 1200), Size(2560, 1440)
};

std::string resolution_text(bool fullscreen)
{
  std::ostringstream out;
  if (fullscreen)
  {
    if (g_config->fullscreen_size == Size(0, 0))
    {
      out << "Desktop";
    }
    else
    {
      out << g_config->fullscreen_size.width << "x" << g_config->fullscreen_size.height;
    }
  }
  else if (g_config->window_maximised)
  {
    out << "Maximised";
  }
  else
  {
    out << g_config->window_size.width << "x" << g_config->window_size.height;
  }
  return out.str();
}

} // namespace

std::string
current_resolution_text()
{
  return resolution_text(g_config->use_fullscreen);
}

std::string
refresh_rate_text(int rate)
{
  if (rate != 0)
  {
    return std::to_string(rate) + " Hz";
  }

  // Auto names the rate the screen ends up at
  SDL_DisplayMode mode;
  if (g_config->fullscreen_size == Size(0, 0))
  {
    if (SDL_GetDesktopDisplayMode(0, &mode) != 0)
    {
      return "Auto";
    }
  }
  else
  {
    SDL_DisplayMode wanted;
    wanted.format = SDL_PIXELFORMAT_UNKNOWN;
    wanted.w = g_config->fullscreen_size.width;
    wanted.h = g_config->fullscreen_size.height;
    wanted.refresh_rate = 0;
    wanted.driverdata = nullptr;
    if (SDL_GetClosestDisplayMode(0, &wanted, &mode) == nullptr)
    {
      return "Auto";
    }
  }

  return (mode.refresh_rate > 0) ? "Auto (" + std::to_string(mode.refresh_rate) + " Hz)" : "Auto";
}

std::string
current_refresh_rate_text()
{
  return refresh_rate_text(g_config->fullscreen_refresh_rate);
}

std::vector<int>
refresh_rates_for(const Size& size)
{
  std::vector<int> rates;

  const int mode_count = SDL_GetNumDisplayModes(0);
  for (int i = 0; i < mode_count; ++i)
  {
    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, i, &mode) != 0 || mode.w != size.width || mode.h != size.height)
    {
      continue;
    }
    if (std::find(rates.begin(), rates.end(), mode.refresh_rate) == rates.end())
    {
      rates.push_back(mode.refresh_rate);
    }
  }

  return rates;
}

ResolutionMenu::ResolutionMenu() :
  m_sizes(),
  m_fullscreen(g_config->use_fullscreen),
  m_refresh_rate_row(-1)
{
  add_label(m_fullscreen ? "Fullscreen Resolution" : "Window Size");
  add_hl();

  std::vector<Size> sizes;
  if (m_fullscreen)
  {
    // SDL lists the largest first, and the smallest come first here
    for (int i = SDL_GetNumDisplayModes(0) - 1; i >= 0; --i)
    {
      SDL_DisplayMode mode;
      if (SDL_GetDisplayMode(0, i, &mode) != 0 || mode.w < MIN_FULLSCREEN_WIDTH || mode.h < MIN_FULLSCREEN_HEIGHT)
      {
        continue;
      }
      const Size size(mode.w, mode.h);
      if (std::find(sizes.begin(), sizes.end(), size) == sizes.end())
      {
        sizes.push_back(size);
      }
    }
  }
  else
  {
    // A size beyond the room the desktop leaves would only be cut back down by the window manager
    SDL_Rect usable;
    if (SDL_GetDisplayUsableBounds(0, &usable) != 0)
    {
      usable.w = 0;
      usable.h = 0;
    }
    for (const Size& size : WINDOW_SIZES)
    {
      if (usable.w <= 0 || (size.width <= usable.w && size.height <= usable.h))
      {
        sizes.push_back(size);
      }
    }

    // A size reached by dragging the window or asked for on the command line still shows as the one in use
    if (std::find(sizes.begin(), sizes.end(), g_config->window_size) == sizes.end())
    {
      sizes.push_back(g_config->window_size);
      std::stable_sort(sizes.begin(), sizes.end(), [](const Size& lhs, const Size& rhs) {
        return lhs.width * lhs.height < rhs.width * rhs.height;
      });
    }
  }

  sizes.push_back(Size(0, 0));
  for (const Size& size : sizes)
  {
    MenuItem& item = add_entry(static_cast<int>(m_sizes.size()), size_text(size));
    if (m_fullscreen && size != Size(0, 0))
    {
      item.set_help("Not every screen shows every size. If it goes black, wait and it switches back");
    }
    m_sizes.push_back(size);
  }

  if (m_fullscreen)
  {
    add_hl();
    add_inactive("Refresh Rate");
    m_refresh_rate_row = static_cast<int>(m_items.size()) - 1;
  }

  add_hl();
  add_back("Back");

  refresh();
}

std::string
ResolutionMenu::size_text(const Size& size) const
{
  if (size == Size(0, 0))
  {
    return m_fullscreen ? "Desktop" : "Maximised";
  }

  return std::to_string(size.width) + "x" + std::to_string(size.height);
}

void
ResolutionMenu::menu_action(MenuItem& item)
{
  const int id = item.get_id();
  if (id < 0 || id >= static_cast<int>(m_sizes.size()))
  {
    return;
  }

  const Size& size = m_sizes[static_cast<size_t>(id)];
  const VideoSetting previous = VideoSetting::current();
  VideoSetting wanted = previous;
  if (m_fullscreen)
  {
    // A rate belongs to the size it was picked with
    wanted.fullscreen_size = size;
    wanted.refresh_rate = 0;
  }
  else
  {
    wanted.maximised = (size == Size(0, 0));
    if (!wanted.maximised)
    {
      wanted.window_size = size;
    }
  }

  if (wanted == previous)
  {
    return;
  }

  wanted.apply();

  // A window can't be left showing nothing, and neither can the desktop's own mode
  if (g_config->use_fullscreen && wanted.fullscreen_size != Size(0, 0))
  {
    VideoModeDialog::ask(previous);
  }
  else
  {
    g_config->save();
  }
}

void
ResolutionMenu::refresh()
{
  const std::string in_use = resolution_text(m_fullscreen);
  for (size_t i = 0; i < m_sizes.size(); ++i)
  {
    const std::string text = size_text(m_sizes[i]);
    get_item_by_id(static_cast<int>(i)).set_text(text == in_use ? "[" + text + "]" : text);
  }

  if (m_refresh_rate_row >= 0)
  {
    // A size with one rate or none to pick from greys the row out
    size_t rates = 0;
    if (g_config->fullscreen_size != Size(0, 0))
    {
      const std::vector<int> found = refresh_rates_for(g_config->fullscreen_size);
      rates = static_cast<size_t>(std::count_if(found.begin(), found.end(), [](int rate) { return rate != 0; }));
    }

    const std::string text = "Refresh Rate: " + current_refresh_rate_text();
    if (rates > 1)
    {
      m_items[static_cast<size_t>(m_refresh_rate_row)] = std::make_unique<ItemGoTo>(text, MenuStorage::REFRESH_RATE_MENU);
    }
    else
    {
      m_items[static_cast<size_t>(m_refresh_rate_row)] = std::make_unique<ItemInactive>(text);
    }
  }

  calculate_width();
}

/* EOF */
