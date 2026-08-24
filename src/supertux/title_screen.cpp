// src/supertux/title_screen.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "supertux/title_screen.hpp"

#include "audio/sound_manager.hpp"
#include "gui/menu.hpp"
#include "gui/menu_manager.hpp"
#include "object/camera.hpp"
#include "object/player.hpp"
#include "supertux/fadein.hpp"
#include "supertux/fadeout.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/textscroller.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

#include <sstream>
#include <version.h>

namespace {

/** How far the notice sits in from the corner of the screen. */
const float COPYRIGHT_MARGIN = 5.0f;

/** Break every line of a text to a width, keeping the breaks already in it. */
std::string wrap_to_screen(const FontPtr& font, const std::string& text, float width)
{
  std::string wrapped;
  std::istringstream lines(text);
  std::string line;

  while (std::getline(lines, line))
  {
    std::string rest = line;
    do
    {
      if (!wrapped.empty())
        wrapped += "\n";
      wrapped += font->wrap_to_width(rest, width, &rest);
    }
    while (!rest.empty());
  }

  return wrapped;
}

} // namespace

TitleScreen::TitleScreen(Savegame& savegame) :
  frame(),
  controller(),
  titlesession(),
  copyright_text(),
  wrapped_copyright(),
  wrapped_width(0)
{
  controller.reset(new CodeController());
  titlesession.reset(new GameSession("levels/misc/menu.stl", savegame));

  Player* player = titlesession->get_current_sector()->player;
  player->set_controller(controller.get());
  player->set_speedlimit(230); //MAX_WALK_XM

  frame = Surface::create("images/engine/menu/frame.png");
  /* Each line here is a whole thought. Where one is too long for the screen it
     is broken up when it is drawn, so the breaks follow the width in use. */
  copyright_text = "SuperTux " PACKAGE_VERSION "\n"
    "Copyright (c) 2003-2016 SuperTux Devel Team\n"
    "This game comes with ABSOLUTELY NO WARRANTY. This is free software, and you "
    "are welcome to redistribute it under certain conditions; see the license "
    "file for details.";
}

void
TitleScreen::make_tux_jump()
{
  static bool jumpWasReleased = true;
  Sector* sector  = titlesession->get_current_sector();
  Player* tux = sector->player;

  controller->update();
  controller->press(Controller::RIGHT);

  // Check if we should press the jump button
  Rectf lookahead = tux->get_bbox();
  lookahead.p2.x += 96;
  bool pathBlocked = !sector->is_free_of_statics(lookahead);
  if ((pathBlocked && jumpWasReleased) || !tux->on_ground()) {
    controller->press(Controller::JUMP);
    jumpWasReleased = false;
  } else {
    jumpWasReleased = true;
  }

  // Wrap around at the end of the level back to the beginning
  if(sector->get_width() - 320 < tux->get_pos().x) {
    sector->activate("main");
    sector->camera->reset(tux->get_pos());
  }
}

TitleScreen::~TitleScreen()
{
}

void
TitleScreen::setup()
{
  Sector* sector = titlesession->get_current_sector();
  if(Sector::current() != sector) {
    sector->play_music(LEVEL_MUSIC);

    /* activate() wants a spawnpoint, which it reads as the top of a big Tux
       and drops a tile for a small one so that either lands feet first on the
       same ground. Tux is already standing where he belongs, so work back to
       the spawnpoint that leaves him there. Keep this in step with the drop
       in Sector::activate. */
    Player* tux = sector->player;
    Vector spawn = tux->get_pos();
    if (!tux->is_big()) spawn.y -= 32;
    sector->activate(spawn);
  }

  MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
  ScreenManager::current()->set_screen_fade(std::unique_ptr<ScreenFade>(new FadeIn(0.25)));
}

void
TitleScreen::leave()
{
  Sector* sector = titlesession->get_current_sector();
  sector->deactivate();
  MenuManager::instance().clear_menu_stack();
}

void
TitleScreen::draw(DrawingContext& context)
{
  Sector* sector  = titlesession->get_current_sector();
  sector->draw(context);

  context.draw_surface_part(frame,
                            Rectf(0, 0, frame->get_width(), frame->get_height()),
                            Rectf(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
                            LAYER_FOREGROUND1);

  /* Breaking the text costs a pass over every character, so it is kept until
     the screen is a different width. */
  if (wrapped_width != SCREEN_WIDTH)
  {
    wrapped_width = SCREEN_WIDTH;
    wrapped_copyright = wrap_to_screen(Resources::small_font, copyright_text,
                                       static_cast<float>(SCREEN_WIDTH) - COPYRIGHT_MARGIN * 2);
  }

  /* Sat against the bottom, so that the extra lines a narrow screen needs grow
     upwards into the picture rather than off the end of it. */
  float height = Resources::small_font->get_text_height(wrapped_copyright);
  context.draw_text(Resources::small_font,
                    wrapped_copyright,
                    Vector(COPYRIGHT_MARGIN, SCREEN_HEIGHT - COPYRIGHT_MARGIN - height),
                    ALIGN_LEFT, LAYER_FOREGROUND1);
}

void
TitleScreen::update(float elapsed_time)
{
  ScreenManager::current()->set_speed(0.6f);
  Sector* sector  = titlesession->get_current_sector();
  sector->update(elapsed_time);

  make_tux_jump();

  // reopen menu if user closed it (so that the app doesn't close when user
  // accidently hit ESC)
  if(!MenuManager::instance().is_active() && !ScreenManager::current()->has_pending_fadeout())
  {
    MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
  }
}

/* EOF */
