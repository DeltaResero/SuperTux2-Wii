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

#include "badguy/flame.hpp"

#include <cmath>

#include "audio/sound_manager.hpp"
#include "badguy/mriceblock.hpp"
#include "badguy/stalactite.hpp"
#include "math/util.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "util/reader_mapping.hpp"

static const std::string FLAME_SOUND = "sounds/flame.flac";

Flame::Flame(const ReaderMapping& reader, const std::string& sprite) :
  BadGuy(reader, sprite, LAYER_FLOATINGOBJECTS,
         "images/objects/lightmap_light/lightmap_light-small.sprite"),
  angle(0),
  radius(),
  speed(),
  liquid(0),
  sound_source()
{
  reader.get("radius", radius, 100.0f);
  reader.get("speed", speed, 2.0f);
  m_col.m_bbox.set_pos(Vector(m_start_position.x + cosf(angle) * radius,
                              m_start_position.y + sinf(angle) * radius));
  m_countMe = false;
  SoundManager::current()->preload(FLAME_SOUND);

  reader.get("sprite", m_sprite_name, m_sprite_name.c_str());
  m_sprite = SpriteManager::current()->create(m_sprite_name);

  set_colgroup_active(COLGROUP_TOUCHABLE);

  m_lightsprite->set_color(Color(0.21f, 0.13f, 0.08f));
  m_glowing = true;
}

void
Flame::active_update(float dt_sec)
{
  angle = fmodf(angle + dt_sec * speed, math::TAU);
  Vector newpos(m_start_position.x + cosf(angle) * radius,
                m_start_position.y + sinf(angle) * radius);
  m_col.set_movement(newpos - get_pos());
  sound_source->set_position(get_pos());

  if (m_sprite->get_action() == "fade" && m_sprite->animation_done()) remove_me();

  // Water puts out fire and lava melts ice once its middle is in, which it has to look for itself as it never touches tiles
  if (liquid && m_col.get_group() != COLGROUP_DISABLED &&
      !Sector::get().is_free_of_tiles(Rectf(m_col.m_bbox.get_middle(), Sizef(1.0f, 1.0f)), false, liquid))
  {
    if (liquid == Tile::WATER)
      freeze();
    else
      ignite();
  }
}

void
Flame::activate()
{
  sound_source = SoundManager::current()->create_sound_source(FLAME_SOUND);
  sound_source->set_position(get_pos());
  sound_source->set_looping(true);
  // Louder than vanilla's, where a flame circling Tux could barely be heard
  sound_source->set_gain(4.0f);
  sound_source->set_close_range();
  sound_source->play();

  // Most orbits never come near water or lava, so look once and spare the check every frame
  liquid = is_freezable() ? Tile::WATER : is_flammable() ? Tile::FIRE : 0;
  if (liquid && Sector::get().is_free_of_tiles(get_reach().grown(1.0f), false, liquid))
    liquid = 0;
}

Rectf
Flame::get_reach() const
{
  const Vector centre = m_start_position + Vector(m_col.m_bbox.get_width(), m_col.m_bbox.get_height()) / 2.0f;
  return Rectf(centre, Sizef(0.0f, 0.0f)).grown(radius);
}

void
Flame::deactivate()
{
  sound_source.reset();
}


void
Flame::kill_fall()
{
}

HitResponse
Flame::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  // A falling stalactite or a shell puts the flame out like an iceball does, and breaks or melts doing it
  auto stalactite = dynamic_cast<Stalactite*>(&badguy);
  auto shell = dynamic_cast<MrIceBlock*>(&badguy);
  if ((stalactite || (shell && shell->is_shell())) && is_freezable()) {
    freeze();
    if (stalactite) stalactite->squish();
    if (shell) shell->melt();
  }
  return BadGuy::collision_badguy(badguy, hit);
}

void
Flame::freeze()
{
  SoundManager::current()->play("sounds/sizzle.flac", get_pos());
  m_sprite->set_action("fade", 1);
  Sector::get().add<SpriteParticle>("images/particles/smoke.sprite",
                                         "default",
                                         m_col.m_bbox.get_middle(), ANCHOR_MIDDLE,
                                         Vector(0, -150), Vector(0,0), LAYER_BACKGROUNDTILES+2);
  set_group(COLGROUP_DISABLED);

  // start dead-script
  run_dead_script();
}

bool
Flame::is_freezable() const
{
  return true;
}

bool
Flame::is_flammable() const
{
  return false;
}

void Flame::stop_looping_sounds()
{
  if (sound_source) {
    sound_source->pause();
  }
}

void Flame::play_looping_sounds()
{
  if (sound_source) {
    sound_source->play();
  }
}

/* EOF */
