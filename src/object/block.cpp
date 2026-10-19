//  SuperTux
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


#include "object/block.hpp"

#include <array>
#include <cmath>

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "badguy/bomb.hpp"
#include "math/random.hpp"
#include "object/bordered.hpp"
#include "object/coin.hpp"
#include "object/growup.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/constants.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/sector.hpp"
#include "util/file_system.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"

static const float BOUNCY_BRICK_MAX_OFFSET = 8;
static const float BOUNCY_BRICK_SPEED = 90;
static const float BUMP_ROTATION_ANGLE = 10;

Block::Block(const ReaderMapping& mapping, const std::string& sprite_file) :
  m_sprite(),
  m_sprite_name(),
  m_bouncing(false),
  m_breaking(false),
  m_bounce_dir(0),
  m_original_y(-1)
{
  mapping.get("x", m_col.m_bbox.get_left());
  mapping.get("y", m_col.m_bbox.get_top());

  std::string sf;
  mapping.get("sprite", sf);
  if (sf.empty() || FileSystem::find(sf).empty()) {
    sf = sprite_file;
  }
  m_sprite = SpriteManager::current()->create(sf);
  m_sprite_name = sf;

  m_col.m_bbox.set_size(32, 32.1f);
  set_group(COLGROUP_STATIC);
  SoundManager::current()->preload("sounds/upgrade.flac");
  SoundManager::current()->preload("sounds/brick.flac");
}

HitResponse
Block::collision(GameObject& other, const CollisionHit& )
{
  auto player = dynamic_cast<Player*> (&other);
  if (player)
  {
    if(player->is_swimboosting())
    {
      hit(*player);
    }
    else if (!player->is_swimboosting() && !player->is_water_jumping() && !player->is_swimming())
    {
      bool x_coordinates_intersect =
        player->get_bbox().get_right() >= m_col.m_bbox.get_left() &&
        player->get_bbox().get_left() <= m_col.m_bbox.get_right();
      if (player->get_bbox().get_top() > m_col.m_bbox.get_bottom() - SHIFT_DELTA &&
          x_coordinates_intersect)
      {
        if (player->get_bbox().get_top() > m_col.m_bbox.get_bottom() - SHIFT_DELTA)
        {
          hit(*player);
        }
      }
    }
  }

  // only interact with other objects if...
  //   1) we are bouncing
  //   2) the object is not portable (either never or not currently)
  //   3) the object is being hit from below (baguys don't get killed for activating boxes)
  auto badguy = dynamic_cast<BadGuy*> (&other);
  auto portable = dynamic_cast<Portable*> (&other);
  auto moving_object = dynamic_cast<MovingObject*> (&other);
  auto bomb = dynamic_cast<Bomb*> (&other);
  bool is_portable = ((portable != nullptr) && portable->is_portable());
  bool is_bomb = (bomb != nullptr); // bombs need to explode, although they are considered portable
  bool hit_mo_from_below = ((moving_object == nullptr) || (moving_object->get_bbox().get_bottom() < (m_col.m_bbox.get_top() + SHIFT_DELTA)));
  if (m_bouncing && (!is_portable || badguy || is_bomb) && hit_mo_from_below) {

    // Badguys get killed
    if (badguy) {
      badguy->kill_fall();
    }

    // Coins get collected
    auto coin = dynamic_cast<Coin*> (&other);
    if (coin) {
      coin->collect();
    }

    //Eggs get jumped
    auto growup = dynamic_cast<GrowUp*> (&other);
    if (growup) {
      growup->do_jump();
    }

  }

  return FORCE_MOVE;
}


void
Block::update(float dt_sec)
{
  if (!m_bouncing)
    return;

  float offset = m_original_y - get_pos().y;
  if (offset > BOUNCY_BRICK_MAX_OFFSET) {
    m_bounce_dir = BOUNCY_BRICK_SPEED;
    m_col.set_movement(Vector(0, m_bounce_dir * dt_sec));
    if (m_breaking){
      break_me();
    }
  } else if (offset < BOUNCY_BRICK_SPEED * dt_sec && m_bounce_dir > 0) {
    m_col.set_movement(Vector(0, offset));
    m_bounce_dir = 0;
    m_bouncing = false;
    m_sprite->set_angle(0);
  } else {
    m_col.set_movement(Vector(0, m_bounce_dir * dt_sec));
  }
}

SurfacePtr
Block::get_resting_surface() const
{
  const Vector pos = get_pos();
  if (m_bouncing || m_breaking || !m_sprite->is_plain() ||
      std::abs(pos.x - std::round(pos.x / 32.0f) * 32.0f) > 0.001f || std::abs(pos.y - std::round(pos.y / 32.0f) * 32.0f) > 0.001f)
  {
    return {};
  }
  // A picture that bends what's behind it, like an emptied block's reflection, can't come from a copy
  SurfacePtr surface = m_sprite->get_current_surface();
  return (surface && surface->get_displacement_texture()) ? SurfacePtr() : surface;
}

void
Block::draw(DrawingContext& context)
{
  // Resting on the grid near the screen, it's bordered by the tiles and blocks beside it
  const SurfacePtr surface = get_resting_surface();
  if (surface && context.transform().between_pixels && VideoSystem::current()->draws_between_pixels() && d_gameobject_manager &&
      context.get_cliprect().grown(32.0f).contains(Rectf(get_pos(), Sizef(32.0f, 32.0f))))
  {
    const int cx = static_cast<int>(std::round(get_pos().x / 32.0f));
    const int cy = static_cast<int>(std::round(get_pos().y / 32.0f));
    std::array<const Surface*, 9> cells;
    for (int k = 0; k < 9; ++k)
    {
      cells[static_cast<size_t>(k)] = (k == 4) ? surface.get() :
                                      bordered::grid_surface_at(*d_gameobject_manager, cx + k % 3 - 1, cy + k / 3 - 1);
    }
    Vector origin;
    Size texels;
    const SurfacePtr page = bordered::copy_of(cells, origin, texels);
    if (page)
    {
      // It draws its joints with tiles, and shares one with a block beside it as tiles do
      const bordered::Open open = { !bordered::block_at(cx - 1, cy), !bordered::block_at(cx, cy - 1),
                                    !bordered::block_at(cx - 1, cy - 1), !bordered::block_at(cx + 1, cy - 1) };
      std::vector<Rectf> srcrects;
      std::vector<Rectf> dstrects;
      bordered::place(get_pos(), Sizef(32.0f, 32.0f), origin, texels.width, texels.height, open, srcrects, dstrects);
      context.push_transform();
      context.transform().part_texels = true;
      context.color().draw_surface_batch(page, std::move(srcrects), std::move(dstrects), Color::WHITE, LAYER_OBJECTS + 1);
      context.pop_transform();
      return;
    }
  }
  m_sprite->draw(context.color(), get_pos(), LAYER_OBJECTS+1);
}

void
Block::start_bounce(GameObject* hitter)
{
  if (m_original_y == -1){
    m_original_y = m_col.m_bbox.get_top();
  }
  m_bouncing = true;
  m_bounce_dir = -BOUNCY_BRICK_SPEED;

  MovingObject* hitter_mo = dynamic_cast<MovingObject*>(hitter);
  if (hitter_mo) {
    float center_of_hitter = hitter_mo->get_bbox().get_middle().x;
    float offset = (m_col.m_bbox.get_middle().x - center_of_hitter)*2 / m_col.m_bbox.get_width();

    // Without this, hitting a multi-coin bonus block from the side (e. g. with
    // an ice block or a snail) would turn the block 90 degrees.
    if (offset > 2 || offset < -2)
      offset = 0;

    m_sprite->set_angle(BUMP_ROTATION_ANGLE*offset);
  }
}

void
Block::start_break(GameObject* hitter)
{
  start_bounce(hitter);
  m_breaking = true;
}

void
Block::break_me()
{
  const auto gravity = Sector::get().get_gravity() * 100;
  Vector pos = get_pos() + Vector(16.0f, 16.0f);

  for (const char* action : {"piece1", "piece2", "piece3", "piece4", "piece5", "piece6"})
  {
    Vector velocity(graphicsRandom.randf(-100, 100),
                    graphicsRandom.randf(-400, -300));
    Sector::get().add<SpriteParticle>(m_sprite->clone(), action,
                                pos, ANCHOR_MIDDLE,
                                velocity, Vector(0, gravity),
                                LAYER_OBJECTS + 1);
  }

  remove_me();
}

void
Block::on_flip(float height)
{
  MovingObject::on_flip(height);
  if (m_original_y != -1) m_original_y = height - m_original_y - get_bbox().get_height();
}

/* EOF */
