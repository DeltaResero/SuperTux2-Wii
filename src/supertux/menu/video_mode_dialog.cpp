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

#include <cmath>
#include <memory>
#include <string>

#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/video_system.hpp"

namespace {

// Long enough for a screen that goes dark to have done so and be noticed
const float KEEP_SECONDS = 15.0f;

std::string prompt(int seconds)
{
  return "Keep this display setting?\nGoing back in " + std::to_string(seconds) +
         (seconds == 1 ? " second" : " seconds");
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

  VideoSystem::current()->apply_config();
  MenuManager::instance().on_window_resize();
  MenuManager::instance().refresh();
}

void
VideoModeDialog::ask(const VideoSetting& previous)
{
  MenuManager::instance().set_dialog(std::make_unique<VideoModeDialog>(previous));
}

VideoModeDialog::VideoModeDialog(const VideoSetting& previous) :
  m_previous(previous),
  m_deadline(g_real_time + KEEP_SECONDS),
  m_counted(static_cast<int>(KEEP_SECONDS))
{
  set_text(prompt(m_counted));
  // Only a kept mode reaches the file, so a game closed on a dark screen doesn't start on one
  add_default_button("Keep", [] { g_config->save(); });
  add_cancel_button("Go Back", [this] { go_back(); });
}

void
VideoModeDialog::go_back()
{
  m_previous.apply();
  g_config->save();
}

void
VideoModeDialog::update()
{
  const int left = static_cast<int>(std::ceil(m_deadline - g_real_time));
  if (left <= 0)
  {
    go_back();
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
