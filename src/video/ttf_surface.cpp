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

#include "video/ttf_surface.hpp"

#include <SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <tuple>
#include <vector>

#include "util/log.hpp"
#include "video/sdl_surface.hpp"
#include "video/surface.hpp"
#include "video/ttf_font.hpp"
#include "video/ttf_surface_manager.hpp"
#include "video/video_system.hpp"

namespace {

using Offsets = std::vector<std::tuple<int, int> >;

// Where an outline this many pixels thick is blitted; up to 2 these are the offsets it always used
Offsets outline_offsets(int size)
{
  Offsets offsets;
  for (int y = -size; y <= size; ++y)
  {
    for (int x = -size; x <= size; ++x)
    {
      const int distance = x * x + y * y;
      if (distance > 0 && distance <= size * size && (size > 2 || distance > (size - 1) * (size - 1)))
      {
        offsets.emplace_back(x, y);
      }
    }
  }
  return offsets;
}

// Where a shadow this many pixels soft is blitted around its offset; up to 2 these are the offsets it always used
Offsets shadow_offsets(int size)
{
  if (size == 1)
  {
    return Offsets{ {0, 0} };
  }

  Offsets offsets;
  for (int y = 1 - size; y < size; ++y)
  {
    for (int x = 1 - size; x < size; ++x)
    {
      const int distance = x * x + y * y;
      if (distance > 0 && distance <= (size - 1) * (size - 1))
      {
        offsets.emplace_back(x, y);
      }
    }
  }
  return offsets;
}

// A decoration a font has at its own size, at the size the text is rendered
int scaled(int size, float scale)
{
  return size > 0 ? std::max(1, static_cast<int>(std::lround(static_cast<float>(size) * scale))) : 0;
}

} // namespace

TTFSurfacePtr
TTFSurface::create(const TTFFont& font, const std::string& text, const Vector& pixel_scale)
{
  // The larger side is rendered at its pixel size and the other squeezed to fit, so nothing is stretched
  const float scale = std::max(pixel_scale.x, pixel_scale.y);
  TTF_Font* ttf_font = font.get_ttf_font(scaled(font.get_font_size(), scale));

  SDLSurfacePtr text_surface(TTF_RenderUTF8_Blended(ttf_font,
                                                    text.c_str(),
                                                    SDL_Color{255, 255, 255, 255}));
  if (!text_surface)
  {
    log_warning << "Couldn't render text '" << text << "' :" << SDL_GetError();
    return std::make_shared<TTFSurface>(SurfacePtr(), Sizef());
  }

  const int border = scaled(font.get_border(), scale);
  const int shadow_size = scaled(font.get_shadow_size(), scale);
  const int shadow_offset = scaled(2, scale);

  // FIXME: handle shadow offset
  int grow = std::max(border * 2, shadow_size * 2);

  SDLSurfacePtr target = SDLSurface::create_rgba(text_surface->w + grow, text_surface->h + grow);

#if !SDL_VERSION_ATLEAST(2,0,5)
  // Perform blitting in ARGB8888, instead of RGBA8888, to avoid bug in older SDL2.
  // https://bugzilla.libsdl.org/show_bug.cgi?id=3159
  target.reset(SDL_ConvertSurfaceFormat(target.get(), SDL_PIXELFORMAT_ARGB8888, 0));
#endif

  { // shadow
    SDL_SetSurfaceAlphaMod(text_surface.get(), 192);
    SDL_SetSurfaceColorMod(text_surface.get(), 0, 0, 0);
    SDL_SetSurfaceBlendMode(text_surface.get(), SDL_BLENDMODE_BLEND);

    if (shadow_size > 0)
    {
      for (const auto& p : shadow_offsets(shadow_size))
      {
        SDL_Rect dstrect{std::get<0>(p) + shadow_offset, std::get<1>(p) + shadow_offset, text_surface->w, text_surface->h};
        SDL_BlitSurface(text_surface.get(), nullptr,
                        target.get(), &dstrect);
      }
    }
  }

  { // outline
    SDL_SetSurfaceAlphaMod(text_surface.get(), 255);
    SDL_SetSurfaceColorMod(text_surface.get(), 0, 0, 0);
    SDL_SetSurfaceBlendMode(text_surface.get(), SDL_BLENDMODE_BLEND);

    for (const auto& p : outline_offsets(border))
    {
      SDL_Rect dstrect{std::get<0>(p), std::get<1>(p), text_surface->w, text_surface->h};
      SDL_BlitSurface(text_surface.get(), nullptr,
                      target.get(), &dstrect);
    }
  }

  { // white core
    SDL_SetSurfaceAlphaMod(text_surface.get(), 255);
    SDL_SetSurfaceColorMod(text_surface.get(), 255, 255, 255);
    SDL_SetSurfaceBlendMode(text_surface.get(), SDL_BLENDMODE_BLEND);

    SDL_Rect dstrect{0, 0, text_surface->w, text_surface->h};

    SDL_BlitSurface(text_surface.get(), nullptr, target.get(), &dstrect);
  }

#if !SDL_VERSION_ATLEAST(2,0,5)
  target.reset(SDL_ConvertSurfaceFormat(target.get(), SDL_PIXELFORMAT_RGBA8888, 0));
#endif

  if (pixel_scale.x != pixel_scale.y)
  {
    target = SDLSurface::shrink(*target,
                                std::max(1, static_cast<int>(std::lround(static_cast<float>(target->w) * pixel_scale.x / scale))),
                                std::max(1, static_cast<int>(std::lround(static_cast<float>(target->h) * pixel_scale.y / scale))));
  }

  SurfacePtr result = Surface::from_texture(VideoSystem::current()->new_texture(*target));
  return std::make_shared<TTFSurface>(result, Sizef(static_cast<float>(target->w) / pixel_scale.x,
                                                    static_cast<float>(target->h) / pixel_scale.y));
}

TTFSurface::TTFSurface(const SurfacePtr& surface, const Sizef& size) :
  m_surface(surface),
  m_size(size)
{
}

/* EOF */
