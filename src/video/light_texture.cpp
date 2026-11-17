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

#include "video/light_texture.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

#include <SDL.h>

#include "video/sdl_surface_ptr.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"

namespace {

// Below this a glow shows its own pixels and above it the lightmap can't tell the difference
const int SIDE_MIN = 16;
const int SIDE_MAX = 256;

struct Curve
{
  float plateau;
  float edge;
};

// Where each glow starts to fade and where it's gone as fractions of its radius measured off the pictures they replace
const Curve CURVES[LIGHT_CURVE_COUNT] = {
  { 0.25f, 0.95f }, // LIGHT_SOFT
  { 0.40f, 1.00f }  // LIGHT_WIDE
};

float
ramp(float r, const Curve& curve)
{
  if (r >= curve.edge) return 0.0f;
  if (r <= curve.plateau) return 1.0f;

  const float t = (r - curve.plateau) / (curve.edge - curve.plateau);
  return 1.0f - t * t * (3.0f - 2.0f * t);
}

// The power of two at or above wanted so a glow is never squeezed below half its pixels
int
side_for(float wanted)
{
  int side = SIDE_MIN;
  while (static_cast<float>(side) < wanted && side < SIDE_MAX)
  {
    side *= 2;
  }
  return side;
}

SurfacePtr
build(const Curve& curve, int width, int height)
{
  SDLSurfacePtr image(SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32));
  if (!image)
  {
    throw std::runtime_error("Couldn't make a light: " + std::string(SDL_GetError()));
  }

  const float half_w = static_cast<float>(width) / 2.0f;
  const float half_h = static_cast<float>(height) / 2.0f;
  Uint8* pixels = static_cast<Uint8*>(image->pixels);

  for (int y = 0; y < height; ++y)
  {
    Uint8* row = pixels + y * image->pitch;
    for (int x = 0; x < width; ++x)
    {
      const float dx = (static_cast<float>(x) + 0.5f - half_w) / half_w;
      const float dy = (static_cast<float>(y) + 0.5f - half_h) / half_h;
      const float r = std::sqrt(dx * dx + dy * dy);

      // White even where it's clear or filtering pulls black into the rim
      row[4 * x + 0] = 255;
      row[4 * x + 1] = 255;
      row[4 * x + 2] = 255;
      row[4 * x + 3] = static_cast<Uint8>(std::lround(ramp(r, curve) * 255.0f));
    }
  }

  return Surface::from_texture(VideoSystem::current()->new_texture(*image));
}

} // namespace

LightTexture::LightTexture() :
  m_glows()
{
}

SurfacePtr
LightTexture::get(LightCurve curve, float width, float height)
{
  const auto key = std::make_tuple(static_cast<int>(curve), side_for(width), side_for(height));
  const auto it = m_glows.find(key);
  if (it != m_glows.end())
  {
    return it->second;
  }

  SurfacePtr glow = build(CURVES[curve], std::get<1>(key), std::get<2>(key));
  m_glows.emplace(key, glow);
  return glow;
}

/* EOF */
