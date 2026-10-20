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

#include "video/sdl/sdl_texture.hpp"

#include <SDL.h>
#include <sstream>

#include "video/sdl/sdl_screen_renderer.hpp"
#include "video/sdl_surface_ptr.hpp"
#include "video/video_system.hpp"

namespace {

uint32_t s_last_serial = 0;

// The scale quality hint's "best" isn't linear, so each texture asks for what the OpenGL renderer would use
void
set_scale_mode(SDL_Texture* texture, const Sampler& sampler)
{
  SDL_SetTextureScaleMode(texture, sampler.get_filter() == GL_NEAREST ? SDL_ScaleModeNearest : SDL_ScaleModeLinear);
}

} // namespace

SDLTexture::SDLTexture(SDL_Texture* texture, int width, int height, const Sampler& sampler) :
  m_texture(texture),
  m_width(width),
  m_height(height),
  m_sampler(sampler),
  m_texel_scale(1.0f, 1.0f),
  m_serial(++s_last_serial)
{
  set_scale_mode(m_texture, m_sampler);
}

SDLTexture::SDLTexture(const SDL_Surface& image, const Sampler& sampler, const Size& image_size) :
  m_texture(),
  m_width(image_size.width > 0 ? image_size.width : image.w),
  m_height(image_size.height > 0 ? image_size.height : image.h),
  m_sampler(sampler),
  m_texel_scale(1.0f, 1.0f),
  m_serial()
{
  reload(image);
}

void
SDLTexture::reload(const SDL_Surface& image)
{
  SDL_Texture* texture = SDL_CreateTextureFromSurface(static_cast<SDLScreenRenderer&>(VideoSystem::current()->get_renderer()).get_sdl_renderer(),
                                                      const_cast<SDL_Surface*>(&image));
  if (!texture)
  {
    std::ostringstream msg;
    msg << "couldn't create texture: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  set_scale_mode(texture, m_sampler);

  if (m_texture)
  {
    SDL_DestroyTexture(m_texture);
  }
  m_texture = texture;
  m_texel_scale = Vector(static_cast<float>(image.w) / static_cast<float>(m_width),
                         static_cast<float>(image.h) / static_cast<float>(m_height));
  m_serial = ++s_last_serial;
}

void
SDLTexture::update(const SDL_Surface& pixels, int x, int y)
{
  // In the texture's own layout, which SDL picked when making it
  Uint32 format = 0;
  SDL_QueryTexture(m_texture, &format, nullptr, nullptr, nullptr);
  SDLSurfacePtr converted;
  const SDL_Surface* source = &pixels;
  if (format != pixels.format->format)
  {
    converted.reset(SDL_ConvertSurfaceFormat(const_cast<SDL_Surface*>(&pixels), format, 0));
    if (!converted.get()) return;
    source = converted.get();
  }
  const SDL_Rect rect{ x, y, pixels.w, pixels.h };
  SDL_UpdateTexture(m_texture, &rect, source->pixels, source->pitch);
  m_serial = ++s_last_serial;
}

SDLTexture::~SDLTexture()
{
  SDL_DestroyTexture(m_texture);
}

/* EOF */
