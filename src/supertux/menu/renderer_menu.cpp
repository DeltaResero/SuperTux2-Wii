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

#include "supertux/menu/renderer_menu.hpp"

#include <cstddef>
#include <memory>
#include <string>

#include "gui/dialog.hpp"
#include "gui/item_action.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"

namespace {

std::string renderer_name(VideoSystem::Enum video)
{
  for (const VideoSystem::Info& info : VideoSystem::get_available_video_systems())
  {
    if (info.value == video)
    {
      return info.name;
    }
  }
  return VideoSystem::get_video_string(video);
}

} // namespace

std::string
current_renderer_text()
{
  const VideoSystem::Enum asked = g_config->video;
  if (asked != VideoSystem::get_requested_video_system())
  {
    return renderer_name(asked) + " (restart to apply)";
  }

  const VideoSystem::Enum running = VideoSystem::get_running_video_system();
  if (asked == VideoSystem::VIDEO_AUTO)
  {
    return renderer_name(asked) + " (" + renderer_name(running) + ")";
  }
  if (running != asked)
  {
    return renderer_name(asked) + " (using " + renderer_name(running) + ")";
  }
  return renderer_name(asked);
}

RendererMenu::RendererMenu() :
  m_video_systems(VideoSystem::get_available_video_systems())
{
  add_label("Renderer");
  add_hl();

  for (size_t i = 0; i < m_video_systems.size(); ++i)
  {
    const VideoSystem::Info& info = m_video_systems[i];
    // Only a renderer the driver refused at this launch is known not to work
    const std::string text = VideoSystem::is_unavailable(info.value) ? info.name + " (not available)" : info.name;
    add_entry(static_cast<int>(i), info.value == g_config->video ? "[" + text + "]" : text)
      .set_help(info.description);
  }

  add_hl();
  add_back("Back");
}

void
RendererMenu::menu_action(MenuItem& item)
{
  const int id = item.get_id();
  if (id < 0 || id >= static_cast<int>(m_video_systems.size()))
  {
    return;
  }

  const VideoSystem::Enum wanted = m_video_systems[static_cast<size_t>(id)].value;
  MenuManager::instance().pop_menu();
  if (wanted == g_config->video)
  {
    return;
  }

  g_config->video = wanted;
  g_config->save();
  MenuManager::instance().refresh();

  // The window and every texture belong to the renderer they were made with
  auto dialog = std::make_unique<Dialog>();
  dialog->set_text("The new renderer takes over the next time the game starts");
  dialog->add_button("OK");
  MenuManager::instance().set_dialog(std::move(dialog));
}

/* EOF */
