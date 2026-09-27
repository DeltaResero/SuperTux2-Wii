//  SuperTux
//  Copyright (C) 2004 Tobas Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#include "supertux/menu/integrations_menu.hpp"

#include "config.h"

#include "audio/sound_manager.hpp"
#include "gui/item_goto.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/item_toggle.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#ifdef ENABLE_DISCORD
#include "sdk/discord.hpp"
#endif
#include "supertux/gameconfig.hpp"
#include "supertux/game_session.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/log.hpp"
#include "video/renderer.hpp"

enum IntegrationsMenuIDs {
#ifdef ENABLE_DISCORD
  MNID_ENABLE_DISCORD
#endif
};

IntegrationsMenu::IntegrationsMenu()
{
  add_label("Integrations");
  add_hl();
#ifdef ENABLE_DISCORD
  add_toggle(MNID_ENABLE_DISCORD, "Enable Discord integration", &g_config->enable_discord)
    .set_help("Sends information to your Discord application about what you're doing in the game.");
#else
  add_inactive("Discord (disabled; not compiled)");
#endif
  add_hl();
  add_back("Back");
}

IntegrationsMenu::~IntegrationsMenu()
{
}

void
IntegrationsMenu::menu_action(MenuItem& item)
{
  switch (item.get_id()) {

#ifdef ENABLE_DISCORD
    case MNID_ENABLE_DISCORD:
      if (g_config->enable_discord)
      {
        DiscordIntegration::getDriver()->init();
      }
      else
      {
        DiscordIntegration::getDriver()->close();
      }
      break;
#endif
    default:
      break;
  }
}

/* EOF */
