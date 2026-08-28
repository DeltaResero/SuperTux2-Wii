// src/object/light.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include "object/light.hpp"

#include "video/drawing_context.hpp"

Light::Light(const Vector& center, const Color& color_) :
  position(center),
  color(color_)
{
}

Light::~Light()
{
}

void
Light::update(float )
{
}

void
Light::draw(DrawingContext& context)
{
  context.draw_light(position, LIGHT_NORMAL, color);
}

/* EOF */
