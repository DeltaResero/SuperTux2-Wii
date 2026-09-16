// src/supertux/menu/video_mode_dialog.cpp
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

#include "supertux/menu/video_mode_dialog.hpp"

#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

#include <cmath>
#include <memory>
#include <sstream>

namespace {

/** Long enough for a screen that goes dark to have done so and be noticed. */
const float KEEP_SECONDS = 15.0f;

std::string prompt(int seconds)
{
  std::ostringstream out;
  out << "Keep this display setting?\nGoing back in " << seconds
      << (seconds == 1 ? " second" : " seconds");
  return out.str();
}

} // namespace

VideoSetting
VideoSetting::current()
{
  VideoSetting setting;
  setting.fullscreen_size = g_config->fullscreen_size;
  setting.refresh_rate = g_config->fullscreen_refresh_rate;
  setting.window_size = g_config->window_size;
  setting.maximised = g_config->window_maximised;
  return setting;
}

bool
VideoSetting::operator==(const VideoSetting& other) const
{
  return fullscreen_size == other.fullscreen_size &&
         refresh_rate == other.refresh_rate &&
         window_size == other.window_size &&
         maximised == other.maximised;
}

void
VideoSetting::apply() const
{
  g_config->fullscreen_size = fullscreen_size;
  g_config->fullscreen_refresh_rate = refresh_rate;
  g_config->window_size = window_size;
  g_config->window_maximised = maximised;

  VideoSystem::current()->get_renderer().apply_config();
  MenuManager::instance().on_window_resize();
  MenuManager::instance().refresh();
}

void
VideoModeDialog::ask(const VideoSetting& previous)
{
  std::unique_ptr<Dialog> dialog(new VideoModeDialog(previous));
  MenuManager::instance().set_dialog(std::move(dialog));
}

VideoModeDialog::VideoModeDialog(const VideoSetting& previous) :
  m_previous(previous),
  m_deadline(real_time + KEEP_SECONDS),
  m_counted(static_cast<int>(KEEP_SECONDS))
{
  set_text(prompt(m_counted));
  add_default_button("Keep");
  add_cancel_button("Go Back", [this] { m_previous.apply(); });
}

void
VideoModeDialog::update()
{
  const int left = static_cast<int>(std::ceil(m_deadline - real_time));

  if (left <= 0)
  {
    m_previous.apply();
    MenuManager::instance().set_dialog({});
    return;
  }

  if (left != m_counted)
  {
    m_counted = left;
    set_text(prompt(left));
  }
}

/* EOF */
