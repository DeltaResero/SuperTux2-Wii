//  SuperTux
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "object/candle.hpp"

#include <vector>

#include "math/random.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

namespace {

// How bright a candle's light is against its colour
const float CANDLE_LIGHT = 0.25f;
// How far a flicker dims it
const float CANDLE_FLICKER = 0.77f;

} // namespace

Candle::Candle(const ReaderMapping& mapping) :
  MovingSprite(mapping, "images/objects/candle/candle.sprite", LAYER_BACKGROUNDTILES+1, COLGROUP_DISABLED),
  ExposedObject<Candle, scripting::Candle>(this),
  burning(true),
  flicker(true),
  lightcolor(1.0f, 1.0f, 1.0f)
{
  mapping.get("burning", burning, true);
  mapping.get("flicker", flicker, true);
  std::vector<float> vColor;
  if (!mapping.get("color", vColor)) vColor = {1.0f, 1.0f, 1.0f};
  mapping.get("layer", m_layer, 0);

  //change the light color if defined
  if (vColor.size() >= 3) {
    lightcolor = Color(vColor);
  }

  if (burning) {
    m_sprite->set_action("on");
  } else {
    m_sprite->set_action("off");
  }

}

void
Candle::draw(DrawingContext& context)
{
  // draw regular sprite
  m_sprite->draw(context.color(), get_pos(), m_layer);

  // draw on lightmap
  if (burning) {
    // draw approx. 1 in 10 frames darker. Makes the candle flicker
    const float strength = (graphicsRandom.rand(10) != 0 || !flicker) ? CANDLE_LIGHT : CANDLE_LIGHT * CANDLE_FLICKER;
    context.light().draw_light(m_col.m_bbox.get_middle(), LIGHT_CANDLE,
                               Color(lightcolor.red * strength, lightcolor.green * strength,
                                     lightcolor.blue * strength, lightcolor.alpha),
                               m_layer);
  }
}

HitResponse
Candle::collision(GameObject&, const CollisionHit& )
{
  return FORCE_MOVE;
}

void
Candle::puff_smoke()
{
  Vector ppos = m_col.m_bbox.get_middle();
  Vector pspeed = Vector(0, -150);
  Vector paccel = Vector(0,0);
  Sector::get().add<SpriteParticle>("images/particles/smoke.sprite",
                                         "default",
                                         ppos, ANCHOR_MIDDLE,
                                         pspeed, paccel,
                                         LAYER_BACKGROUNDTILES+2);
}

bool
Candle::get_burning() const
{
  return burning;
}

void
Candle::set_burning(bool burning_)
{
  if (burning == burning_) return;
  burning = burning_;
  if (burning_) {
    m_sprite->set_action("on");
  } else {
    m_sprite->set_action("off");
  }
  //puff smoke for flickering light sources only
  if (flicker) puff_smoke();
}

/* EOF */
