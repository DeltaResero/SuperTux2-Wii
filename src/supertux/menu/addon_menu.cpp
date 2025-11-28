// src/supertux/menu/addon_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/addon_menu.hpp"

#include "addon/addon.hpp"
#include "addon/addon_manager.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/download_dialog.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

#include <algorithm>

AddonMenu::AddonMenu(bool /*language_pack_mode*/, bool /*auto_install_langpack*/) :
  m_addon_manager(*AddonManager::current()),
  m_installed_addons(),
  m_repository_addons(),
  m_addons_enabled(nullptr)
{
  refresh();
}

AddonMenu::~AddonMenu()
{
  delete[] m_addons_enabled;
}

void
AddonMenu::refresh()
{
  m_installed_addons = m_addon_manager.get_installed_addons();
  m_repository_addons = m_addon_manager.get_repository_addons();

  std::sort(m_installed_addons.begin(), m_installed_addons.end());
  std::sort(m_repository_addons.begin(), m_repository_addons.end());

  rebuild_menu();
}

void
AddonMenu::rebuild_menu()
{
  clear();

  add_label("Add-ons");
  add_hl();

  add_entry(MNID_CHECK_ONLINE, "Check Online");

  add_hl();

  delete[] m_addons_enabled;
  m_addons_enabled = new bool[m_installed_addons.size()];

  int i = 0;
  for(const auto& id : m_installed_addons)
  {
    try {
      const Addon& addon = m_addon_manager.get_installed_addon(id);

      m_addons_enabled[i] = false;
      for (const auto& cfg_addon : g_config->addons)
      {
        if (cfg_addon.id == addon.get_id() && cfg_addon.enabled)
        {
          m_addons_enabled[i] = true;
          break;
        }
      }

      add_toggle(MNID_ADDON_LIST_START + i, addon.get_title(), &m_addons_enabled[i]);
    } catch (std::exception&) {
      // Ignore missing addons
    }
    i++;
  }

  for(const auto& id : m_repository_addons)
  {
    try {
      const Addon& addon = m_addon_manager.get_repository_addon(id);
      add_entry(MNID_ADDON_LIST_START + i, addon.get_title() + " (install)");
    } catch (std::exception&) {
      // Ignore missing addons
    }
    i++;
  }

  if (m_installed_addons.empty() && m_repository_addons.empty())
  {
    add_inactive("No Add-ons available");
  }

  add_hl();
  add_back("Back");
}

void
AddonMenu::menu_action(MenuItem* item)
{
  if (item->id == MNID_CHECK_ONLINE)
  {
    check_online();
  }
  else if (item->id >= MNID_ADDON_LIST_START)
  {
    int index = item->id - MNID_ADDON_LIST_START;
    if (index < (int)m_installed_addons.size())
    {
      try {
        const Addon& addon = m_addon_manager.get_installed_addon(m_installed_addons[index]);
        toggle_addon(addon);
      } catch (...) {}
    }
    else
    {
      index -= m_installed_addons.size();
      if (index < (int)m_repository_addons.size())
      {
        try {
          const Addon& addon = m_addon_manager.get_repository_addon(m_repository_addons[index]);
          install_addon(addon);
        } catch (...) {}
      }
    }
  }
}

void
AddonMenu::check_online()
{
  try
  {
    TransferStatusPtr status = m_addon_manager.request_check_online();
    status->then([this](bool success)
    {
      if (success)
      {
        refresh();
      }
    });
    std::unique_ptr<DownloadDialog> dialog(new DownloadDialog(status));
    dialog->set_title("Downloading Add-On Repository Index");
    MenuManager::instance().set_dialog(std::move(dialog));
  }
  catch (std::exception& e)
  {
    log_warning << "Check for available Add-ons failed: " << e.what() << std::endl;
  }
}

void
AddonMenu::install_addon(const Addon& addon)
{
  std::string addon_id = addon.get_id();
  std::string text = "Installing " + addon.get_title();

  try {
    TransferStatusPtr status = m_addon_manager.request_install_addon(addon_id);

    std::unique_ptr<DownloadDialog> dialog(new DownloadDialog(status));
    dialog->set_title(text);

    status->then([this, addon_id](bool success)
    {
      if (success)
      {
        try
        {
          m_addon_manager.enable_addon(addon_id);
        }
        catch(const std::exception& err)
        {
          log_warning << "Enabling add-on failed: " << err.what() << std::endl;
        }
        refresh();
      }
    });

    MenuManager::instance().set_dialog(std::move(dialog));
  } catch (std::exception& e) {
    log_warning << "Failed to start download: " << e.what() << std::endl;
  }
}

void
AddonMenu::toggle_addon(const Addon& addon)
{
  bool enabled = false;
  bool found = false;
  for (auto& cfg_addon : g_config->addons)
  {
    if (cfg_addon.id == addon.get_id())
    {
      cfg_addon.enabled = !cfg_addon.enabled;
      enabled = cfg_addon.enabled;
      found = true;
      break;
    }
  }

  if (!found)
  {
    Config::Addon cfg_addon;
    cfg_addon.id = addon.get_id();
    cfg_addon.enabled = true;
    g_config->addons.push_back(cfg_addon);
    enabled = true;
  }

  if (enabled)
    m_addon_manager.enable_addon(addon.get_id());
  else
    m_addon_manager.disable_addon(addon.get_id());

  g_config->save();
}

bool
AddonMenu::addon_visible(const Addon& /*addon*/) const
{
  return true;
}

// EOF
