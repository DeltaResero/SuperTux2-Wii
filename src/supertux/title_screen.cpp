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


#include <config.h>

#include "audio/sound_manager.hpp"
#include "gui/menu.hpp"
#include "gui/menu_manager.hpp"
#include "object/camera.hpp"
#include "object/player.hpp"
#include "object/bonus_block.hpp"
#include "object/brick.hpp"
#include "supertux/fadein.hpp"
#include "supertux/fadeout.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/collision.hpp"
#include "supertux/sector.hpp"
#include "supertux/textscroller.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

#include <ctime>
#include <sstream>

namespace {

/** How far the notice sits in from the corner of the screen. Kept narrow: the
    last line of the warranty is within a few units of the width the game is
    narrowest at, and a wider inset pushes its final word onto a line of its
    own. */
const float COPYRIGHT_MARGIN = 3.0f;

/** The jump, measured off a running game rather than read off the constants:
    Tux leaves the ground at 510 units against the level's gravity of 1000
    while walking at 230, and tops out 128 up, which is four tiles exactly. */
const float JUMP_SPEED = 510.6f;
const float WALK_SPEED = 230.0f;
const float ARC_STEP = 1.0f / 64.0f;

/** How keen he is on the bonus blocks, drawn fresh for each run at the level.
    At the bottom of the range he ignores them altogether, which is what the
    title screen has always done; at the top he takes about a quarter of the
    ones he could reach. */
const float LEAST_EAGER = 0.0f;
const float MOST_EAGER = 0.25f;

/** Adding the golden ratio and wrapping at one spreads successive values as far
    apart as they will go, so his keenness and his choices never fall into a
    rhythm. Where in the sequence a run starts comes from the clock, stirred
    first, because one second to the next moves the start too little to show. */
const float SPACING_STEP = 0.6180339887f;

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
  m_savegame(savegame),
  frame(),
  controller(),
  titlesession(),
  copyright_text(),
  wrapped_copyright(),
  wrapped_width(0),
  jump_was_released(true),
  eagerness(0.0f),
  crate_eagerness(0.0f),
  spacing(static_cast<float>((static_cast<unsigned int>(std::time(nullptr))
                              * 2654435761u) % 1000u) / 1000.0f),
  last_block(NULL),
  taking_block(false)
{
  controller.reset(new CodeController());
  create_session();

  frame = Surface::create("images/engine/menu/frame.png");
  /* Each line here is a whole thought. Where one is too long for the screen it
     is broken up when it is drawn, so the breaks follow the width in use. */
  copyright_text = "SuperTux " PACKAGE_VERSION "\n"
    "Copyright (c) 2003-2016 SuperTux Devel Team\n"
    "This game comes with ABSOLUTELY NO WARRANTY. This is free software, and you "
    "are welcome to redistribute it under certain conditions; see the license "
    "file for details.";
}

float
TitleScreen::next_spacing()
{
  spacing += SPACING_STEP;
  if (spacing >= 1.0f) spacing -= 1.0f;
  return spacing;
}

bool
TitleScreen::worth_jumping_for(const Block& block)
{
  /* Crates keep their own appetite, drawn separately, because a Tux who is
     keen on the bonus blocks this lap needn't be keen on breaking things. A
     Tux too small to break one never gets that far: block_in_arc has already
     passed them over. */
  const float keen = dynamic_cast<const Brick*>(&block) != NULL
                     ? crate_eagerness : eagerness;

  return next_spacing() < keen;
}

Vector
TitleScreen::arc_head(const Rectf& body, float crown, float gravity, float t)
{
  /* Where the top of his head would be t seconds into a jump taken now, less
     the tile he still has to walk: without it he leaves the ground a tile
     short of the block every time. */
  return Vector(crown + WALK_SPEED * t - 32.0f,
                body.p1.y - JUMP_SPEED * t + 0.5f * gravity * t * t);
}

const Block*
TitleScreen::block_in_arc(const Sector& sector, const Player& tux) const
{
  const Rectf& body = tux.get_bbox();
  /* Taken from the sector rather than written down here, so that a map which
     sets its own gravity is still aimed at correctly. */
  const float gravity = sector.get_gravity() * 100.0f;
  const float climb = JUMP_SPEED / gravity;

  /* The top of his head, and only that: he is a tile wide, so asking whether
     any part of him would touch a block answers yes a whole tile before he is
     under it, which is a jump into its side rather than its underside. */
  const float crown = (body.p1.x + body.p2.x) * 0.5f;

  /* Everything the climb passes through, in one box. An object outside it
     cannot be reached however long the arc is walked, and throwing those out
     on four comparisons is what keeps the dynamic_cast below down to the
     handful of objects actually near him. */
  const Rectf sweep(crown - 32.0f,
                    body.p1.y - JUMP_SPEED * JUMP_SPEED / (2.0f * gravity),
                    crown + WALK_SPEED * climb - 32.0f,
                    body.p1.y);

  /* How far up the climb gets before the ground or the scenery ends it. Left
     until something is actually worth aiming at, because on open ground there
     is nothing for it to rule out and it is the dearest part of this. */
  float stopped = climb + ARC_STEP;
  bool ground_known = false;

  const Block* target = NULL;

  for (const auto& object : sector.moving_objects)
  {
    if (!object->is_valid()) continue;
    if (object->get_group() != COLGROUP_STATIC) continue;
    if (!collision::intersects(sweep, object->get_bbox())) continue;

    /* Only now is it worth asking what the object is. Only blocks with
       something left in them count: an empty one is scenery, and a crate is no
       use to a Tux too small to break it. */
    const Block* block = dynamic_cast<const Block*>(object);
    if (block == NULL || block->is_spent()) continue;
    if (dynamic_cast<const Brick*>(block) != NULL && !tux.is_big()) continue;

    if (!ground_known)
    {
      ground_known = true;
      for (float t = ARC_STEP; t <= climb; t += ARC_STEP)
      {
        const Vector head = arc_head(body, crown, gravity, t);
        if (!sector.is_free_of_tiles(Rectf(head.x, head.y,
                                           head.x + 1.0f, head.y + 1.0f)))
        {
          stopped = t;
          break;
        }
      }
    }

    /* Whichever he would reach first is the one he is going for. */
    for (float t = ARC_STEP; t < stopped; t += ARC_STEP)
    {
      if (block->get_bbox().contains(arc_head(body, crown, gravity, t)))
      {
        stopped = t;
        target = block;
        break;
      }
    }
  }

  return target;
}

void
TitleScreen::make_tux_jump()
{
  Sector* sector  = titlesession->get_current_sector();
  Player* tux = sector->player;

  controller->update();
  controller->press(Controller::RIGHT);

  /* Off the ground he's committed either way, since the jump is held to its
     full height and there's no second one to spend, so neither of these is
     worth working out until his feet are back under him. */
  bool pathBlocked = false;

  if (tux->on_ground())
  {
    // Check if we should press the jump button
    Rectf lookahead = tux->get_bbox();
    lookahead.p2.x += 96;
    pathBlocked = !sector->is_free_of_statics(lookahead);

    /* Whether to go for a block is settled once, as it comes into reach, and
       stands until a different one does. Asked every frame instead, the answer
       would come up yes within a few of them however seldom he meant to. */
    const Block* target = block_in_arc(*sector, *tux);
    if (target != last_block) {
      last_block = target;
      taking_block = target != NULL && worth_jumping_for(*target);
    }
  }

  if (((pathBlocked || taking_block) && jump_was_released)
      || !tux->on_ground()) {
    controller->press(Controller::JUMP);
    jump_was_released = false;
  } else {
    jump_was_released = true;
  }

  // Wrap around at the end of the level back to the beginning
  if(sector->get_width() - 320 < tux->get_pos().x) {
    sector->activate("main");
    sector->camera->reset(tux->get_pos());

    /* A fresh lap, and a fresh appetite for the blocks along it. */
    eagerness = LEAST_EAGER + next_spacing() * (MOST_EAGER - LEAST_EAGER);
    crate_eagerness = LEAST_EAGER + next_spacing() * (MOST_EAGER - LEAST_EAGER);
  }
}

TitleScreen::~TitleScreen()
{
}

void
TitleScreen::create_session()
{
  titlesession.reset(new GameSession("levels/misc/menu.stl", m_savegame));

  Player* player = titlesession->get_current_sector()->player;
  player->set_controller(controller.get());
  player->set_speedlimit(230); //MAX_WALK_XM
}

void
TitleScreen::setup()
{
  if(!titlesession) {
    create_session();
  }

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

  jump_was_released = true;
  last_block = NULL;
  taking_block = false;
  eagerness = LEAST_EAGER + next_spacing() * (MOST_EAGER - LEAST_EAGER);
  crate_eagerness = LEAST_EAGER + next_spacing() * (MOST_EAGER - LEAST_EAGER);

  MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
  ScreenManager::current()->set_screen_fade(std::unique_ptr<ScreenFade>(new FadeIn(0.25)));
}

void
TitleScreen::leave()
{
  Sector* sector = titlesession->get_current_sector();
  sector->deactivate();
  MenuManager::instance().clear_menu_stack();

  /* The menu is played over a whole level, and this screen is never popped,
     so holding it would keep that level's artwork for the rest of the run.
     It is built again on the way back in. */
  titlesession.reset();
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
