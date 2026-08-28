// src/video/light_texture.cpp
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

#include "video/light_texture.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <SDL.h>

#include "supertux/globals.hpp"
#include "video/sdl_surface_ptr.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"

namespace {

/** How much smaller the lightmap is drawn than the screen. Both renderers
    keep their own copy of this; being out of step with them only makes the
    glow coarser or finer than it needs to be, never wrong. */
const int LIGHTMAP_DIV = 5;

/** No glow is made smaller than this, or it shows its own edges, nor larger,
    since past it the lightmap is too coarse to tell the difference. */
const int SIDE_MIN = 64;
const int SIDE_MAX = 256;

/** Both glows are the same shape: solid out to `plateau`, then a smooth fall
    to nothing at `edge`, both given as a fraction of the radius. Measured off
    the pictures they replace, which agree with this to about one percent. */
float ramp(float r, float plateau, float edge)
{
  if(r >= edge) return 0.0f;
  if(r <= plateau) return 1.0f;

  const float t = (r - plateau) / (edge - plateau);
  return 1.0f - (t * t * (3.0f - 2.0f * t));
}

/** The biggest glow worth making. Detail past what the lightmap can show is
    wasted, so a small screen does not carry a large picture. */
int size_cap()
{
  int side = SIDE_MIN;
  while(side < SCREEN_WIDTH / LIGHTMAP_DIV && side < SIDE_MAX)
    side *= 2;
  return side;
}

} // namespace

LightTexture::LightTexture() :
  m_round(),
  m_wide(),
  m_cap(0)
{
}

SurfacePtr
LightTexture::get(LightSize size)
{
  const int cap = size_cap();
  if(cap != m_cap)
  {
    m_cap = cap;
    m_round.clear();
    m_wide.clear();
  }

  /* A light drawn smaller than its picture throws most of it away, so make
     the picture no bigger than the light. Going the other way costs nothing,
     which is why the largest sizes share one. */
  const int side = std::min(static_cast<int>(size), m_cap);

  const bool wide = (size == LIGHT_MEDIUM);
  Glows& glows = wide ? m_wide : m_round;

  Glows::iterator it = glows.find(side);
  if(it != glows.end())
    return it->second;

  SurfacePtr made = wide ? build(side, 0.40f, 1.00f)
                         : build(side, 0.25f, 0.95f);
  glows[side] = made;
  return made;
}

SurfacePtr
LightTexture::build(int side, float plateau, float edge) const
{
  SDLSurfacePtr image(SDL_CreateRGBSurfaceWithFormat(0, side, side, 32,
                                                     SDL_PIXELFORMAT_RGBA32));
  if(!image)
    throw std::runtime_error("Couldn't build a light: out of memory");

  const float half = side / 2.0f;
  Uint8* pixels = static_cast<Uint8*>(image->pixels);

  for(int y = 0; y < side; ++y)
  {
    Uint8* row = pixels + y * image->pitch;
    for(int x = 0; x < side; ++x)
    {
      const float dx = (static_cast<float>(x) + 0.5f) - half;
      const float dy = (static_cast<float>(y) + 0.5f) - half;
      const float r = std::sqrt(dx * dx + dy * dy) / half;

      /* White the whole way out, even where it is completely clear. The
         filter mixes in whatever colour the empty pixels carry whenever it
         samples the rim, and black out there rings every light in the game. */
      row[4 * x + 0] = 255;
      row[4 * x + 1] = 255;
      row[4 * x + 2] = 255;
      row[4 * x + 3] = static_cast<Uint8>(ramp(r, plateau, edge) * 255.0f);
    }
  }

  return Surface::create(VideoSystem::current()->new_texture(image.get()));
}

/* EOF */
