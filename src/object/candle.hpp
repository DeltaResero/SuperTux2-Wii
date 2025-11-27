// src/object/candle.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_CANDLE_HPP
#define HEADER_SUPERTUX_OBJECT_CANDLE_HPP

#include "object/moving_sprite.hpp"
#include "scripting/candle.hpp"
#include "scripting/exposed_object.hpp"

/**
 * A burning candle: Simple, scriptable level decoration.
 */
class Candle : public MovingSprite,
               public ExposedObject<Candle, scripting::Candle>
{
public:
  Candle(const ReaderMapping& lisp);
  virtual void draw(DrawingContext& context);

  HitResponse collision(GameObject& other, const CollisionHit& hit);

  /**
   * @name Scriptable Methods
   * @{
   */
  void puff_smoke(); /**< spawn a puff of smoke */
  bool get_burning() const; /**< returns true if candle is lighted */
  void set_burning(bool burning); /**< true: light candle, false: extinguish candle */
  /**
   * @}
   */
  std::string get_class() const {
    return "candle";
  }
  std::string get_display_name() const {
    return _("Candle");
  }

  virtual ObjectSettings get_settings();
  virtual void after_editor_set();

private:
  bool burning; /**< true if candle is currently lighted */
  bool flicker; /**< true if candle light is to flicker */
  Color lightcolor; /**< determines color or light given off */
  SpritePtr candle_light_1; /**< drawn to lightmap */
  SpritePtr candle_light_2; /**< drawn to lightmap (alternative image) */

};

#endif // HEADER_SUPERTUX_OBJECT_CANDLE_HPP

// EOF
