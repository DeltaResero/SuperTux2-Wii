// src/supertux/menu/addon_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_ADDON_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_ADDON_MENU_HPP

#include "gui/menu.hpp"

class Addon;
class AddonManager;

class AddonMenu : public Menu
{
private:
  enum {
    MNID_CHECK_ONLINE,
    MNID_NOTHING_NEW,
    MNID_ADDON_LIST_START = 10
  };

private:
  AddonManager& m_addon_manager;
  std::vector<std::string> m_installed_addons;
  std::vector<std::string> m_repository_addons;
  bool* m_addons_enabled;

public:
  // Parameters kept for compatibility with existing callers, but ignored
  AddonMenu(bool language_pack_mode = false, bool auto_install_langpack = false);
  ~AddonMenu();

  void refresh() override;
  void menu_action(MenuItem* item) override;
  void check_online();
  void install_addon(const Addon& addon);
  void toggle_addon(const Addon& addon);

private:
  void rebuild_menu();
  bool addon_visible(const Addon& addon) const;

private:
  AddonMenu(const AddonMenu&);
  AddonMenu& operator=(const AddonMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_ADDON_MENU_HPP

// EOF
