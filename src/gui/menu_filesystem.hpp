// src/gui/menu_filesystem.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_MENU_FILESYSTEM_HPP
#define HEADER_SUPERTUX_GUI_MENU_FILESYSTEM_HPP

#include "gui/menu.hpp"

class FileSystemMenu : public Menu
{
public:
  FileSystemMenu(std::string* filename_, const std::vector<std::string>& extensions_);
  ~FileSystemMenu();

  void menu_action(MenuItem* item) override;

private:
  std::string* filename;
  std::string directory;
  std::vector<std::string> extensions;
  std::vector<std::string> directories;
  std::vector<std::string> files;

  void refresh_items();
  bool has_right_suffix(const std::string& file) const;

  FileSystemMenu(const FileSystemMenu&);
  FileSystemMenu& operator=(const FileSystemMenu&);
};

#endif // HEADER_SUPERTUX_GUI_MENU_FILESYSTEM_HPP

// EOF
