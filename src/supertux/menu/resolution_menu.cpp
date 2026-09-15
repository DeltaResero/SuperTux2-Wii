// src/supertux/menu/resolution_menu.cpp
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

#include "supertux/menu/resolution_menu.hpp"

#include "supertux/menu/menu_storage.hpp"

#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <tuple>

namespace {

/* The smallest mode worth offering. The view is a fixed 600 units tall
   whatever the mode, so a screen with fewer lines than this squeezes every
   glyph down with it until the text cannot be read. */
const int MIN_FULLSCREEN_WIDTH = 640;
const int MIN_FULLSCREEN_HEIGHT = 480;

/** Sizes a connected screen claims, and how many modes it has at each.

    A display server offers sizes the screen never advertised, expecting
    something to scale them. Where nothing does, the screen shows nothing.
    Empty means there was nothing to read and every mode stands. */
std::map<std::pair<int, int>, int> screen_reported_sizes()
{
  std::map<std::pair<int, int>, int> sizes;

  std::error_code err;
  const std::filesystem::path drm("/sys/class/drm");
  if (!std::filesystem::is_directory(drm, err))
  {
    return sizes;
  }

  for (const auto& entry : std::filesystem::directory_iterator(drm, err))
  {
    std::ifstream status(entry.path() / "status");
    std::string state;
    if (!(status >> state) || state != "connected")
    {
      continue;
    }

    std::ifstream modes(entry.path() / "modes");
    std::string line;
    while (std::getline(modes, line))
    {
      int width = 0;
      int height = 0;
      if (sscanf(line.c_str(), "%dx%d", &width, &height) == 2)
      {
        sizes[{width, height}] += 1;
      }
    }
  }

  return sizes;
}

/** As many rates per size as the screen has modes for it, nearest the
    desktop's first. */
std::set<std::tuple<int, int, int>> offered_modes(int display_mode_count)
{
  std::set<std::tuple<int, int, int>> offered;

  const auto reported = screen_reported_sizes();
  if (reported.empty())
  {
    return offered;
  }

  SDL_DisplayMode desktop;
  if (SDL_GetDesktopDisplayMode(0, &desktop) != 0)
  {
    desktop.refresh_rate = 60;
  }

  for (const auto& size : reported)
  {
    std::vector<int> rates;
    for (int i = 0; i < display_mode_count; ++i)
    {
      SDL_DisplayMode mode;
      if (SDL_GetDisplayMode(0, i, &mode) != 0) continue;
      if (mode.w != size.first.first || mode.h != size.first.second) continue;
      if (std::find(rates.begin(), rates.end(), mode.refresh_rate) == rates.end())
      {
        rates.push_back(mode.refresh_rate);
      }
    }

    std::stable_sort(rates.begin(), rates.end(),
                     [&desktop](int a, int b) {
                       return std::abs(a - desktop.refresh_rate) <
                              std::abs(b - desktop.refresh_rate);
                     });

    if (rates.size() > static_cast<size_t>(size.second))
    {
      rates.resize(static_cast<size_t>(size.second));
    }

    for (int rate : rates)
    {
      offered.insert({size.first.first, size.first.second, rate});
    }
  }

  return offered;
}

} // namespace

std::vector<int> refresh_rates_for(const Size& size)
{
  std::vector<int> rates;

  const int display_mode_count = SDL_GetNumDisplayModes(0);
  const auto offered = offered_modes(display_mode_count);

  for (int i = 0; i < display_mode_count; ++i)
  {
    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, i, &mode) != 0) continue;
    if (mode.w != size.width || mode.h != size.height) continue;
    if (!offered.empty() &&
        offered.find({mode.w, mode.h, mode.refresh_rate}) == offered.end())
      continue;
    if (std::find(rates.begin(), rates.end(), mode.refresh_rate) == rates.end())
    {
      rates.push_back(mode.refresh_rate);
    }
  }

  return rates;
}

std::string current_refresh_rate_text()
{
  if (g_config->fullscreen_refresh_rate == 0)
  {
    return "Auto";
  }

  std::ostringstream out;
  out << g_config->fullscreen_refresh_rate << " Hz";
  return out.str();
}

std::string current_resolution_text()
{
  std::ostringstream out;

  if (g_config->use_fullscreen)
  {
    if (g_config->fullscreen_size == Size(0, 0))
    {
      out << "Desktop";
    }
    else
    {
      out << g_config->fullscreen_size.width << "x"
          << g_config->fullscreen_size.height;
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

ResolutionMenu::ResolutionMenu() :
  m_sizes(),
  m_refresh_rates(),
  m_fullscreen(g_config->use_fullscreen)
{
  add_label(m_fullscreen ? "Fullscreen Resolution" : "Window Size");
  add_hl();

  const std::string in_use = current_resolution_text();

  auto add_resolution = [this, &in_use](const Size& size, int refresh_rate,
                                        const std::string& text) {
    const int id = static_cast<int>(m_sizes.size());
    m_sizes.push_back(size);
    m_refresh_rates.push_back(refresh_rate);
    add_entry(id, text == in_use ? "[" + text + "]" : text);
  };

  if (m_fullscreen)
  {
    /* Sizes only; the rate has a list of its own. */
    const int display_mode_count = SDL_GetNumDisplayModes(0);
    const auto offered = offered_modes(display_mode_count);

    std::vector<Size> sizes;
    for (int i = 0; i < display_mode_count; ++i)
    {
      SDL_DisplayMode mode;
      if (SDL_GetDisplayMode(0, i, &mode) != 0)
      {
        continue;
      }

      if (mode.w < MIN_FULLSCREEN_WIDTH || mode.h < MIN_FULLSCREEN_HEIGHT)
        continue;

      if (!offered.empty() &&
          offered.find({mode.w, mode.h, mode.refresh_rate}) == offered.end())
        continue;

      const Size size(mode.w, mode.h);
      if (std::find(sizes.begin(), sizes.end(), size) == sizes.end())
      {
        sizes.push_back(size);
      }
    }

    for (const auto& size : sizes)
    {
      std::ostringstream out;
      out << size.width << "x" << size.height;
      add_resolution(size, 0, out.str());
    }

    add_resolution(Size(0, 0), 0, "Desktop");
  }
  else
  {
    /* Sizes a window is worth being offered at. A window cannot be larger
       than the desktop it sits on, so a size beyond that is offered only to
       be cut back down by the window manager. */
    std::vector<Size> window_sizes;
    window_sizes.push_back(Size(640, 480));
    window_sizes.push_back(Size(854, 480));
    window_sizes.push_back(Size(800, 600));
    window_sizes.push_back(Size(1280, 720));
    window_sizes.push_back(Size(1280, 800));
    window_sizes.push_back(Size(1440, 900));
    window_sizes.push_back(Size(1920, 1080));
    window_sizes.push_back(Size(1920, 1200));
    window_sizes.push_back(Size(2560, 1440));

    /* The room left once panels have taken their share. A size beyond it
       comes back cut down, and that becomes the size in use. */
    SDL_Rect usable;
    if (SDL_GetDisplayUsableBounds(0, &usable) != 0)
    {
      SDL_DisplayMode desktop;
      usable.w = (SDL_GetDesktopDisplayMode(0, &desktop) == 0) ? desktop.w : 0;
      usable.h = desktop.h;
    }

    if (usable.w > 0)
    {
      window_sizes.erase(std::remove_if(window_sizes.begin(), window_sizes.end(),
                                        [&usable](const Size& size) {
                                          return size.width > usable.w ||
                                                 size.height > usable.h;
                                        }),
                         window_sizes.end());
    }

    /* A size reached by dragging, or asked for on the command line, is
       still shown as the one in use. */
    if (std::find(window_sizes.begin(), window_sizes.end(),
                  g_config->window_size) == window_sizes.end())
    {
      window_sizes.push_back(g_config->window_size);
      std::sort(window_sizes.begin(), window_sizes.end(),
                [](const Size& lhs, const Size& rhs) {
                  return lhs.width * lhs.height < rhs.width * rhs.height;
                });
    }

    for (const auto& size : window_sizes)
    {
      std::ostringstream out;
      out << size.width << "x" << size.height;
      add_resolution(size, 0, out.str());
    }

    /* Left to the window manager, as fullscreen can be left to the screen. */
    add_resolution(Size(0, 0), 0, "Maximised");
  }

  if (m_fullscreen)
  {
    add_hl();
    add_submenu("Refresh Rate: " + current_refresh_rate_text(),
                MenuStorage::REFRESH_RATE_MENU);
  }

  add_hl();
  add_back("Back");
}

void
ResolutionMenu::menu_action(MenuItem* item)
{
  const size_t index = static_cast<size_t>(item->id);
  if (index >= m_sizes.size())
  {
    return;
  }

  if (m_fullscreen)
  {
    /* A rate belongs to the size it was picked under. */
    g_config->fullscreen_size = m_sizes[index];
    g_config->fullscreen_refresh_rate = 0;
  }
  else if (m_sizes[index] == Size(0, 0))
  {
    g_config->window_maximised = true;
  }
  else
  {
    g_config->window_maximised = false;
    g_config->window_size = m_sizes[index];
  }

  VideoSystem::current()->get_renderer().apply_config();
  MenuManager::instance().on_window_resize();

  /* The row that leads here names the size in use, so the menu behind this
     one is told to read it again before it comes back into view. */
  /* The rows naming both are read back from the settings. */
  MenuManager::instance().refresh();
}

void
ResolutionMenu::refresh()
{
  /* So that choosing one shows up at once. */
  const std::string in_use = current_resolution_text();

  for (const auto& item : items)
  {
    if (item->id >= 0 && static_cast<size_t>(item->id) < m_sizes.size())
    {
      const Size& size = m_sizes[static_cast<size_t>(item->id)];

      std::ostringstream out;
      if (size == Size(0, 0))
      {
        out << (m_fullscreen ? "Desktop" : "Maximised");
      }
      else
      {
        out << size.width << "x" << size.height;
      }

      item->text = (out.str() == in_use) ? "[" + out.str() + "]" : out.str();
    }
    else if (item->text.rfind("Refresh Rate: ", 0) == 0)
    {
      item->text = "Refresh Rate: " + current_refresh_rate_text();
    }
  }
}

/* EOF */
