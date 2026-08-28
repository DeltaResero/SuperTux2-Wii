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

/** Makes the round white glow every light in the game is drawn from, so it
    does not have to be kept as a picture. Two are enough for all five sizes:
    the light every badguy carries has a wider bright middle than the rest and
    gets its own, and one serves the other four. */
class LightTexture final
{
public:
  LightTexture();

  /** The glow a light of this size is drawn from, made the first time it is
      asked for. */
  SurfacePtr get(LightSize size);

private:
  SurfacePtr build(float plateau, float edge) const;

  SurfacePtr m_round;
  SurfacePtr m_wide;
  /** How many pixels across the two above were made. Zero until the first
      ask, and revisited when the window changes size. */
  int m_side;

private:
  LightTexture(const LightTexture&) = delete;
  LightTexture& operator=(const LightTexture&) = delete;
};

#endif

/* EOF */
