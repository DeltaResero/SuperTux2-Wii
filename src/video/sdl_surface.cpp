//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/sdl_surface.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <SDL_image.h>

#include "io/sdl_file.hpp"
#include "util/log.hpp"

namespace {

// Which source pixels each pixel of a shorter side covers, and how much of each
void box_weights(int src_size, int dst_size, std::vector<int>& first, std::vector<int>& count, std::vector<float>& weights)
{
  const float step = static_cast<float>(src_size) / static_cast<float>(dst_size);
  first.resize(dst_size);
  count.resize(dst_size);
  weights.clear();
  for (int d = 0; d < dst_size; ++d)
  {
    const float low = static_cast<float>(d) * step;
    const float high = low + step;
    first[d] = static_cast<int>(low);
    count[d] = std::min(src_size, static_cast<int>(std::ceil(high))) - first[d];
    for (int s = first[d]; s < first[d] + count[d]; ++s)
    {
      weights.push_back(std::min(high, static_cast<float>(s + 1)) - std::max(low, static_cast<float>(s)));
    }
  }
}

} // namespace

SDLSurfacePtr
SDLSurface::create_rgba(int width, int height)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 rmask = 0xff000000;
  Uint32 gmask = 0x00ff0000;
  Uint32 bmask = 0x0000ff00;
  Uint32 amask = 0x000000ff;
#else
  Uint32 rmask = 0x000000ff;
  Uint32 gmask = 0x0000ff00;
  Uint32 bmask = 0x00ff0000;
  Uint32 amask = 0xff000000;
#endif
  SDLSurfacePtr surface(SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask));
  if (!surface) {
    std::ostringstream out;
    out << "failed to create SDL_Surface: " << SDL_GetError();
    throw std::runtime_error(out.str());
  }

  return surface;
}

SDLSurfacePtr
SDLSurface::from_file(const std::string& filename)
{
  log_debug << "loading image: " << filename << std::endl;
  SDLSurfacePtr surface(IMG_Load_RW(get_SDLRWops(filename), 1));
  if (!surface)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    return surface;
  }
}

void
SDLSurface::fill_clear_pixels(SDL_Surface& surface)
{
  if (surface.format->format != SDL_PIXELFORMAT_RGBA32)
  {
    return;
  }

  if (SDL_MUSTLOCK(&surface)) SDL_LockSurface(&surface);
  const int width = surface.w;
  const int height = surface.h;
  const auto pixel = [&surface](int x, int y) { return static_cast<Uint8*>(surface.pixels) + y * surface.pitch + x * 4; };
  const auto at = [width](int x, int y) { return static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x); };

  std::vector<Uint8> known(static_cast<size_t>(width) * static_cast<size_t>(height));
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      known[at(x, y)] = pixel(x, y)[3] > 0;
    }
  }

  // As far out as a blend reaches when the picture is drawn smaller
  for (int ring = 0; ring < 2; ++ring)
  {
    std::vector<Uint8> next = known;
    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        if (known[at(x, y)])
        {
          continue;
        }

        int red = 0, green = 0, blue = 0, count = 0;
        for (int ny = std::max(y - 1, 0); ny <= std::min(y + 1, height - 1); ++ny)
        {
          for (int nx = std::max(x - 1, 0); nx <= std::min(x + 1, width - 1); ++nx)
          {
            if (known[at(nx, ny)])
            {
              const Uint8* in = pixel(nx, ny);
              red += in[0];
              green += in[1];
              blue += in[2];
              ++count;
            }
          }
        }
        if (count > 0)
        {
          Uint8* out = pixel(x, y);
          out[0] = static_cast<Uint8>(red / count);
          out[1] = static_cast<Uint8>(green / count);
          out[2] = static_cast<Uint8>(blue / count);
          next[at(x, y)] = 1;
        }
      }
    }
    known.swap(next);
  }
  if (SDL_MUSTLOCK(&surface)) SDL_UnlockSurface(&surface);
}

SDLSurfacePtr
SDLSurface::shrink(const SDL_Surface& surface, int width, int height)
{
  // Every format is read as bytes in R, G, B, A order, the same order create_rgba() writes
  SDLSurfacePtr converted;
  const SDL_Surface* src = &surface;
  if (surface.format->format != SDL_PIXELFORMAT_RGBA32)
  {
    converted.reset(SDL_ConvertSurfaceFormat(const_cast<SDL_Surface*>(&surface), SDL_PIXELFORMAT_RGBA32, 0));
    if (!converted)
    {
      std::ostringstream out;
      out << "failed to convert SDL_Surface: " << SDL_GetError();
      throw std::runtime_error(out.str());
    }
    src = converted.get();
  }
  SDLSurfacePtr dst = create_rgba(width, height);

  std::vector<int> first_x, count_x, first_y, count_y;
  std::vector<float> weights_x, weights_y;
  box_weights(src->w, width, first_x, count_x, weights_x);
  box_weights(src->h, height, first_y, count_y, weights_y);

  if (SDL_MUSTLOCK(src)) SDL_LockSurface(const_cast<SDL_Surface*>(src));
  if (SDL_MUSTLOCK(dst.get())) SDL_LockSurface(dst.get());

  // Across first, into colours multiplied by alpha so a clear pixel adds nothing to its neighbours
  std::vector<float> across(static_cast<size_t>(src->h) * static_cast<size_t>(width) * 4);
  for (int sy = 0; sy < src->h; ++sy)
  {
    const Uint8* row = static_cast<const Uint8*>(src->pixels) + sy * src->pitch;
    float* out = &across[static_cast<size_t>(sy) * static_cast<size_t>(width) * 4];
    const float* weight = weights_x.data();
    for (int x = 0; x < width; ++x, out += 4)
    {
      const Uint8* in = row + first_x[x] * 4;
      float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
      for (int k = 0; k < count_x[x]; ++k, in += 4)
      {
        const float covered = weight[k] * static_cast<float>(in[3]);
        red += covered * static_cast<float>(in[0]);
        green += covered * static_cast<float>(in[1]);
        blue += covered * static_cast<float>(in[2]);
        alpha += covered;
      }
      weight += count_x[x];
      out[0] = red; out[1] = green; out[2] = blue; out[3] = alpha;
    }
  }

  // Then down, and back to plain colours
  const float area = static_cast<float>(src->w) / static_cast<float>(width) *
                     static_cast<float>(src->h) / static_cast<float>(height);
  std::vector<float> sum(static_cast<size_t>(width) * 4);
  const float* weight = weights_y.data();
  for (int y = 0; y < height; ++y)
  {
    std::fill(sum.begin(), sum.end(), 0.0f);
    for (int k = 0; k < count_y[y]; ++k)
    {
      const float* in = &across[static_cast<size_t>(first_y[y] + k) * static_cast<size_t>(width) * 4];
      for (size_t i = 0; i < sum.size(); ++i)
      {
        sum[i] += weight[k] * in[i];
      }
    }
    weight += count_y[y];

    Uint8* out = static_cast<Uint8*>(dst->pixels) + y * dst->pitch;
    for (int x = 0; x < width; ++x, out += 4)
    {
      const float* in = &sum[static_cast<size_t>(x) * 4];
      const float alpha = in[3];
      out[0] = static_cast<Uint8>(alpha > 0.0f ? in[0] / alpha + 0.5f : 0.0f);
      out[1] = static_cast<Uint8>(alpha > 0.0f ? in[1] / alpha + 0.5f : 0.0f);
      out[2] = static_cast<Uint8>(alpha > 0.0f ? in[2] / alpha + 0.5f : 0.0f);
      out[3] = static_cast<Uint8>(alpha / area + 0.5f);
    }
  }

  if (SDL_MUSTLOCK(dst.get())) SDL_UnlockSurface(dst.get());
  if (SDL_MUSTLOCK(src)) SDL_UnlockSurface(const_cast<SDL_Surface*>(src));
  return dst;
}

/* EOF */
