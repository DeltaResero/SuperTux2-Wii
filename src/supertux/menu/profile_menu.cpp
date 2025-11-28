// src/supertux/menu/profile_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2008 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/profile_menu.hpp"

#include <boost/format.hpp>
#include <sstream>

#include "gui/menu_manager.hpp"
#include "gui/menu_item.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/gettext.hpp"

ProfileMenu::ProfileMenu()
{
  add_label("Select Profile");
  add_hl();
  for(int i = 1; i <= 5; ++i)
  {
    std::ostringstream out;
    if (i == g_config->profile)
    {
      out << str(boost::format("[Profile %s]") %i);
    }
    else
    {
      out << str(boost::format("Profile %s") %i);
    }
    add_entry(i, out.str());
  }

  add_hl();
  add_back("Back");
}

void
ProfileMenu::menu_action(MenuItem* item)
{
  g_config->profile = item->id;
  MenuManager::instance().clear_menu_stack();
}

// EOF
