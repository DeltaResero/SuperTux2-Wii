//  SuperTux
//  Copyright (C) 2013 LMH <lmh.0013@gmail.com>
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

#include "badguy/ghostflame.hpp"

#include <algorithm>
#include <typeinfo>

#include "object/ambient_light.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/player_status.hpp"
#include "supertux/sector.hpp"

static const float FADE_TIME = 0.5f;

// Where the bright part of Earth Tux's lamp falls, from his top corner on the side he faces
static const float LAMP_BEHIND = 10.0f;
static const float LAMP_REACH = 471.0f;
static const float LAMP_ABOVE = 88.0f;
static const float LAMP_BELOW = 78.0f;

Ghostflame::Ghostflame(const ReaderMapping& reader) :
  Flame(reader, "images/creatures/flame/ghostflame.sprite"),
  m_fade(-1.0f)
{
  m_lightsprite->set_color(Color(0.21f, 0.00f, 0.21f));
}

void
Ghostflame::active_update(float dt_sec)
{
  Flame::active_update(dt_sec);

  if (m_fade >= 0.0f)
  {
    m_fade += dt_sec;
    m_sprite->set_alpha(std::max(0.0f, 1.0f - m_fade / FADE_TIME));
    if (m_fade >= FADE_TIME)
      remove_me();
    return;
  }
  if (in_bright_light())
  {
    m_fade = 0.0f;
    m_glowing = false;
    set_group(COLGROUP_DISABLED);
    run_dead_script();
  }
}

bool
Ghostflame::in_bright_light() const
{
  for (auto* object : Sector::get().get_objects_by_type_index(typeid(AmbientLight)))
    if (static_cast<AmbientLight*>(object)->get_ambient_light() == Color::WHITE)
      return true;

  const Player& tux = Sector::get().get_player();
  if (tux.get_status().bonus != EARTH_BONUS || tux.is_dying())
    return false;
  const Rectf& body = tux.get_bbox();
  const Rectf beam = tux.m_dir == Direction::RIGHT ?
    Rectf(body.get_left() - LAMP_BEHIND, body.get_top() - LAMP_ABOVE,
          body.get_left() + LAMP_REACH, body.get_top() + LAMP_BELOW) :
    Rectf(body.get_right() - LAMP_REACH, body.get_top() - LAMP_ABOVE,
          body.get_right() + LAMP_BEHIND, body.get_top() + LAMP_BELOW);
  return beam.contains(m_col.m_bbox.get_middle());
}

bool
Ghostflame::is_flammable() const
{
  return false;
}

bool
Ghostflame::is_freezable() const
{
  return false;
}

/* EOF */
