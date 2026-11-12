//  SuperTux
//  Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_DECAL_HPP
#define HEADER_SUPERTUX_OBJECT_DECAL_HPP

#include <string>

#include "math/sizef.hpp"
#include "object/moving_sprite.hpp"
#include "scripting/decal.hpp"
#include "squirrel/exposed_object.hpp"
#include "supertux/artwork_interface.hpp"
#include "supertux/timer.hpp"

class ReaderMapping;

/** A decorative image, perhaps part of the terrain */
class Decal final : public MovingSprite,
                    public ExposedObject<Decal, scripting::Decal>,
                    public ArtworkInterface
{
  friend class FlipLevelTransformer;

public:
  Decal(const ReaderMapping& reader);
  ~Decal() override;

  virtual HitResponse collision(GameObject& , const CollisionHit& ) override { return FORCE_MOVE; }

  virtual std::string get_class() const override { return "decal"; }

  virtual void draw(DrawingContext& context) override;

  virtual void release_artwork() override;
  virtual void reacquire_artwork() override;
  virtual void update(float dt_sec) override;

  virtual void on_flip(float height) override;

  void fade_in(float fade_time);
  void fade_out(float fade_time);
  void fade_sprite(const std::string& new_sprite, float fade_time);

private:
  std::string m_default_action;
  bool m_solid;
  Flip m_flip;
  SpritePtr m_fade_sprite;

  /** How the sprite looked when its pictures were put down */
  std::string m_put_down_action;
  float m_put_down_alpha;
  Timer m_fade_timer;
  Timer m_sprite_timer;
  bool m_visible;

  /** Drawn at this size rather than the picture's own, when given */
  Sizef m_size;

private:
  Decal(const Decal&) = delete;
  Decal& operator=(const Decal&) = delete;
};

#endif

/* EOF */
