// src/gui/menu_filesystem.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "gui/menu_filesystem.hpp"

#include <filesystem>

#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "gui/item_action.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "util/string_util.hpp"

FileSystemMenu::FileSystemMenu(std::string* filename_, const std::vector<std::string>& extensions_) :
  filename(filename_),
  directory(),
  extensions(extensions_),
  directories(),
  files()
{
  directory = FileSystem::dirname(*filename);
  if (!FileSystem::exists(directory)) {
    directory = "/"; //The filename is probably included in an old add-on.
  }

  refresh_items();
}

FileSystemMenu::~FileSystemMenu()
{
}

void
FileSystemMenu::refresh_items()
{
  items.clear();
  directories.clear();
  files.clear();
  directory = FileSystem::normalize(directory);

  add_label(directory);
  add_hl();

  int item_id = 0;

  // Do not allow leaving the data directory
  if (directory != "/") {
    directories.push_back("..");
    add_entry(item_id, "[..]");
    item_id++;
  }

  // Load directories and files
  std::string search_path = FileSystem::find(directory);
  if (!search_path.empty() && std::filesystem::is_directory(search_path)) {
      for (const auto& entry : std::filesystem::directory_iterator(search_path)) {
          std::string filename = entry.path().filename().string();
          std::string filepath = FileSystem::join(directory, filename);

          if (entry.is_directory()) {
              directories.push_back(filename);
              add_entry(item_id, "[" + filename + "]");
              item_id++;
          } else if (has_right_suffix(filename)) {
              files.push_back(filename);
              add_entry(item_id, filename);
              item_id++;
          }
      }
  }

  add_hl();
  add_back(_("Cancel"));

  active_item = 2;

  // Re-center menu
  on_window_resize();
}

bool
FileSystemMenu::has_right_suffix(const std::string& file) const
{
  for (const auto& extension : extensions) {
    if(StringUtil::has_suffix(file, extension))
    {
      return true;
    }
  }
  return false;
}

void
FileSystemMenu::menu_action(MenuItem* item)
{
  if (item->id >= 0) {
    size_t id = item->id;
    if (id < directories.size()) {
      directory = FileSystem::join(directory, directories[id]);
      refresh_items();
    } else {
      id -= directories.size();
      if (id < files.size()) {
        *filename = FileSystem::join(directory, files[id]);
        MenuManager::instance().pop_menu();
      } else {
        log_warning << "Selected invalid file or directory" << std::endl;
      }
    }
  }
}

// EOF
