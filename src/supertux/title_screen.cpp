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
#include <cmath>
#include <ctime>
#include <optional>

#include "gui/menu_manager.hpp"
#include "object/bonus_block.hpp"
#include "object/brick.hpp"
#include "object/camera.hpp"
#include "object/music_object.hpp"
#include "object/player.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/game_session.hpp"
#include "supertux/level.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "video/compositor.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"

namespace {

// How fast his walking jump leaves the ground once the game's stepping has taken its share
const float JUMP_SPEED = 515.4f;
const float WALK_SPEED = 230.0f;
const float ARC_STEP = 1.0f / 64.0f;

// The share of the blocks in reach he goes for is drawn between these each lap
const float LEAST_EAGER = 0.0f;
const float MOST_EAGER = 0.25f;

// Adding the golden ratio and wrapping at 1 keeps successive draws apart
const float SPACING_STEP = 0.6180339887f;

} // namespace

TitleScreen::TitleScreen(Savegame& savegame) :
  m_savegame(savegame),
  m_frame(),
  m_controller(std::make_unique<CodeController>()),
  m_titlesession(),
  m_copyright_text("SuperTux " PACKAGE_VERSION "\n"
    "Copyright (c) 2003-2021 SuperTux Devel Team\n"
    "This game comes with ABSOLUTELY NO WARRANTY. This is free software, and you are\n"
    "welcome to redistribute it under certain conditions; see the license file for details."),
  m_videosystem_name(VideoSystem::current()->get_name()),
  m_jump_was_released(true),
  m_eagerness(0.0f),
  m_crate_eagerness(0.0f),
  m_spacing(static_cast<float>((static_cast<unsigned int>(std::time(nullptr)) * 2654435761u) % 1000u) / 1000.0f),
  m_last_block(nullptr),
  m_taking_block(false)
{
  create_session();
}

void
TitleScreen::create_session()
{
  m_frame = Surface::from_file("images/engine/menu/frame.png");
  m_titlesession = std::make_unique<GameSession>("levels/misc/menu.stl", m_savegame);

  Player& player = m_titlesession->get_current_sector().get_player();
  player.set_controller(m_controller.get());
  player.set_speedlimit(230); //MAX_WALK_XM

  m_jump_was_released = true;
  m_last_block = nullptr;
  m_taking_block = false;
  start_lap();
}

float
TitleScreen::next_spacing()
{
  m_spacing += SPACING_STEP;
  if (m_spacing >= 1.0f) m_spacing -= 1.0f;
  return m_spacing;
}

void
TitleScreen::start_lap()
{
  m_eagerness = LEAST_EAGER + next_spacing() * (MOST_EAGER - LEAST_EAGER);
  m_crate_eagerness = LEAST_EAGER + next_spacing() * (MOST_EAGER - LEAST_EAGER);
}

const Block*
TitleScreen::block_in_arc(const Sector& sector, const Player& tux) const
{
  const Rectf body = tux.get_bbox();
  const float gravity = sector.get_gravity() * 100.0f;
  const float reach = JUMP_SPEED * JUMP_SPEED / (2.0f * gravity);

  // The middle of his head has to come up under the block, or he'd meet its side instead
  const float crown = body.get_middle().x;

  const Block* target = nullptr;
  float soonest = JUMP_SPEED / gravity;

  const auto consider = [&](const Block& block) {
    if (!block.is_valid() || block.is_spent()) return;

    const Rectf& box = block.get_bbox();
    const float gap = body.get_top() - box.get_bottom();
    if (gap < 0.0f || gap >= reach) return;

    // When his head gets up to the block's underside and where it is by then
    const float t = (JUMP_SPEED - sqrtf(JUMP_SPEED * JUMP_SPEED - 2.0f * gravity * gap)) / gravity;
    const float x = crown + WALK_SPEED * t;
    if (x < box.get_left() || x >= box.get_right() || t >= soonest) return;

    // The scenery mustn't end the climb first
    for (float step = ARC_STEP; step < t; step += ARC_STEP)
    {
      const float head_x = crown + WALK_SPEED * step;
      const float head_y = body.get_top() - JUMP_SPEED * step + 0.5f * gravity * step * step;
      if (!sector.is_free_of_tiles(Rectf(head_x, head_y, head_x + 1.0f, head_y + 1.0f))) return;
    }

    soonest = t;
    target = &block;
  };

  for (const auto& block : sector.get_objects_by_type<BonusBlock>())
    consider(block);

  // A crate is no use to a Tux too small to break it
  if (tux.is_big())
  {
    for (const auto& brick : sector.get_objects_by_type<Brick>())
      consider(brick);
  }

  return target;
}

void
TitleScreen::make_tux_jump()
{
  Sector& sector  = m_titlesession->get_current_sector();
  Player& tux = sector.get_player();

  m_controller->update();
  m_controller->press(Control::RIGHT);

  // He can't start a jump in the air, so what's ahead only matters once he's back on the ground
  bool pathBlocked = false;
  if (tux.on_ground())
  {
    Rectf lookahead = tux.get_bbox();
    lookahead.set_right(lookahead.get_right() + 96);
    lookahead.set_bottom(lookahead.get_bottom() - 2);
    pathBlocked = !sector.is_free_of_statics(lookahead);

    // Settled once as a block comes into reach, as asking every frame would soon come up yes whatever the odds
    const Block* target = block_in_arc(sector, tux);
    if (target != m_last_block)
    {
      m_last_block = target;
      const float keen = dynamic_cast<const Brick*>(target) ? m_crate_eagerness : m_eagerness;
      m_taking_block = target && next_spacing() < keen;
    }
  }
  if (((pathBlocked || m_taking_block) && m_jump_was_released) || tux.m_fall_mode == Player::FallMode::JUMPING) {
    m_controller->press(Control::JUMP);
    m_jump_was_released = false;
  } else {
    m_jump_was_released = true;
  }

  // Wrap around at the end of the level back to the beginning
  if (sector.get_width() - 320 < tux.get_pos().x) {
    sector.activate("main");
    sector.get_camera().reset(tux.get_pos());
    start_lap();
  }
}

TitleScreen::~TitleScreen()
{
}

void
TitleScreen::setup()
{
  if (!m_titlesession) {
    create_session();
  }

  Sector& sector = m_titlesession->get_current_sector();
  if (Sector::current() != &sector) {
    auto& music = sector.get_singleton_by_type<MusicObject>();
    music.play_music(LEVEL_MUSIC);
    // activate() drops a small Tux by a tile from a spawnpoint, so undo that for where he stands
    const Player& tux = sector.get_player();
    Vector spawn = tux.get_pos();
    if (!tux.is_big())
      spawn.y -= 32;
    sector.activate(spawn);
  }

  MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
  ScreenManager::current()->set_screen_fade(std::make_unique<FadeToBlack>(FadeToBlack::FADEIN, 0.25f));
}

std::optional<Vector>
TitleScreen::get_player_pos() const
{
  if (!m_titlesession) return std::nullopt;
  return m_titlesession->get_player_pos();
}

void
TitleScreen::leave()
{
  Sector& sector = m_titlesession->get_current_sector();
  sector.deactivate();
  MenuManager::instance().clear_menu_stack();

  // This screen is never popped, so its level and frame would stay for the whole run and are built again on the way back
  m_titlesession.reset();
  m_frame.reset();
}

void
TitleScreen::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();

  Sector& sector  = m_titlesession->get_current_sector();
  sector.draw(context);

  context.color().draw_surface_scaled(m_frame,
                                      Rectf(0, 0, static_cast<float>(context.get_width()), static_cast<float>(context.get_height())),
                                      LAYER_FOREGROUND1);

  context.push_transform();
  context.use_ui_space();

  // Placed by its own height, so a change of font size can't push its last line off, and inset from the bottom as from the side
  const float notice_top = static_cast<float>(context.get_height()) - 5.0f - Resources::small_font->get_text_height(m_copyright_text);
  context.color().draw_text(Resources::small_font,
                            m_copyright_text,
                            Vector(5.0f, notice_top),
                            ALIGN_LEFT, LAYER_FOREGROUND1);

  // Beside the version, the one line of the notice short enough to leave it room
  context.color().draw_text(Resources::small_font,
                            m_videosystem_name,
                            Vector(static_cast<float>(context.get_width()) - 5.0f, notice_top),
                            ALIGN_RIGHT, LAYER_FOREGROUND1);

  context.pop_transform();
}

void
TitleScreen::update(float dt_sec, const Controller& controller)
{
  ScreenManager::current()->set_speed(0.6f);
  Sector& sector  = m_titlesession->get_current_sector();
  sector.update(dt_sec);

  BIND_SECTOR(sector);
  make_tux_jump();

  // reopen menu if user closed it (so that the app doesn't close when user
  // accidently hit ESC)
  if (!MenuManager::instance().is_active() && !ScreenManager::current()->has_pending_fadeout())
  {
    MenuManager::instance().set_menu(MenuStorage::MAIN_MENU);
  }
}

/* EOF */
