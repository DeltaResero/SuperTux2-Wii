//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#ifndef HEADER_SUPERTUX_VIDEO_LIGHT_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_LIGHT_TEXTURE_HPP

#include <map>
#include <tuple>

#include "video/surface_ptr.hpp"

/** How wide a round light falls on the level in pixels and named after the pictures they stand in for */
enum LightSize
{
  LIGHT_TINY = 64,
  LIGHT_SMALL = 128,
  LIGHT_MEDIUM = 384,
  LIGHT_NORMAL = 512,
  LIGHT_LARGE = 1024
};

/** How a glow fades from its bright middle out to nothing */
enum LightCurve
{
  /** A small bright middle fading most of the way out */
  LIGHT_SOFT,
  /** A wider middle fading just as gently for what a badguy carries */
  LIGHT_WIDE,
  LIGHT_CURVE_COUNT
};

/** The white glows every light is drawn from and made rather than loaded */
class LightTexture final
{
public:
  LightTexture();

  /** The glow for a light covering width by height pixels of the lightmap and made the first time it's asked for */
  SurfacePtr get(LightCurve curve, float width, float height);

private:
  /** Never let go before the textures are because a frame's drawing still points at the glows it used */
  std::map<std::tuple<int, int, int>, SurfacePtr> m_glows;

private:
  LightTexture(const LightTexture&) = delete;
  LightTexture& operator=(const LightTexture&) = delete;
};

#endif

/* EOF */
