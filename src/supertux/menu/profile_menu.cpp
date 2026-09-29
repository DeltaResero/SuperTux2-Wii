//  SuperTux
//  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/profile_menu.hpp"

#include <filesystem>
#include <format>
#include <sstream>

#include "gui/dialog.hpp"
#include "gui/menu_manager.hpp"
#include "gui/menu_item.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/file_system.hpp"


ProfileMenu::ProfileMenu()
{
  add_label("Select Profile");
  add_hl();
  for (int i = 1; i <= 5; ++i)
  {
    std::ostringstream out;
    if (i == g_config->profile)
    {
      out << std::format("[Profile {}]", i);
    }
    else
    {
      out << std::format("Profile {}", i);
    }
    add_entry(i, out.str());
  }
  add_hl();
  add_entry(6, "Reset profile");
  add_entry(7, "Reset all profiles");

  add_hl();
  add_back("Back");
}

void
ProfileMenu::menu_action(MenuItem& item)
{
  const auto& id = item.get_id();
  if(id <= 5)
  {
    g_config->profile = item.get_id();
  }
  else if(id == 6)
  {
    Dialog::show_confirmation("Deleting your profile will reset your game progress. Are you sure?", [this]() {
      delete_savegames(g_config->profile);
    });
  }
  else if(id == 7)
  {
    Dialog::show_confirmation("This will reset your game progress on all profiles. Are you sure?", [this]() {
      for (int i = 1; i <= 5; i++) {
        delete_savegames(i);
      }
    });
  }
  MenuManager::instance().clear_menu_stack();
}

void
ProfileMenu::delete_savegames(int idx) const
{
  const auto& profile_path = "profile" + std::to_string(idx);
  std::error_code ec;
  for (const std::string& filename : FileSystem::enumerate(profile_path))
  {
    std::string filepath = FileSystem::join(profile_path, filename);
    std::filesystem::remove(FileSystem::write_path(filepath), ec);
  }
  std::filesystem::remove(FileSystem::write_path(profile_path), ec);
}

/* EOF */
