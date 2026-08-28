// src/video/light_texture.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2026 SuperTux2-Wii contributors
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

#include "video/surface_ptr.hpp"

/** The sizes a light comes in, named after the pictures they stand in for.
    The number is how wide the light falls on the level, in pixels. */
enum LightSize
{
  LIGHT_TINY   = 64,
  LIGHT_SMALL  = 128,
  LIGHT_MEDIUM = 384,
  LIGHT_NORMAL = 512,
  LIGHT_LARGE  = 1024
};

/** How a glow fades from its bright middle out to nothing, measured off the
    pictures each one replaces. A size no longer decides this, because Tux's
    headlamp picks a shape without being any of the sizes above. */
enum LightCurve
{
  LIGHT_SOFT,  /**< a small bright middle, fading most of the way out */
  LIGHT_WIDE,  /**< a wider middle fading just as gently: what a badguy carries */
  LIGHT_CURVE_COUNT
};

/** Makes the white glow every light in the game is drawn from, so it does not
    have to be kept as a picture. Each shape is made at the sizes actually
    asked for, which on a normal screen is four pictures in all. */
class LightTexture final
{
public:
  LightTexture();

  /** The glow a light this shape and size is drawn from, made the first time
      it is asked for, at the shape it will be drawn at. The size given is how
      big the light falls on the level; the picture is never made larger. */
  SurfacePtr get(LightCurve curve, int width, int height);

private:
  SurfacePtr build(int width, int height, float plateau, float edge) const;

  typedef std::map<std::pair<int, int>, SurfacePtr> Glows;

  /** Kept per size rather than made once and shrunk to fit. Squeezing a big
      picture into a small light samples a few of its pixels and picks
      different ones as the light drifts, which sparkles crawling across a
      dark room show up as a flicker. */
  Glows m_glows[LIGHT_CURVE_COUNT];
  /** The largest picture worth making, from how coarse the lightmap is.
      Zero until the first ask, and revisited when the window changes size. */
  int m_cap;

private:
  LightTexture(const LightTexture&) = delete;
  LightTexture& operator=(const LightTexture&) = delete;
};

#endif

/* EOF */
