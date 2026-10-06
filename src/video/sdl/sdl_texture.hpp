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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_TEXTURE_HPP

#include "video/texture.hpp"

#include <SDL.h>

#include "math/size.hpp"
#include "math/vector.hpp"
#include "video/sampler.hpp"

struct SDL_Texture;

class SDLTexture final : public Texture
{
public:
  SDLTexture(SDL_Texture* texture, int width, int height, const Sampler& sampler);
  SDLTexture(const SDL_Surface& sdl_surface, const Sampler& sampler, const Size& image_size = Size());
  ~SDLTexture() override;

  virtual int get_texture_width() const override { return m_width; }
  virtual int get_texture_height() const override { return m_height; }

  virtual int get_image_width() const override { return m_width; }
  virtual int get_image_height() const override { return m_height; }

  virtual void reload(const SDL_Surface& image) override;

  SDL_Texture *get_texture() const { return m_texture; }
  const Sampler& get_sampler() const { return m_sampler; }

  /** How many stored pixels stand for one of the picture's, below 1 when fewer were kept */
  const Vector& get_texel_scale() const { return m_texel_scale; }

private:
  SDL_Texture* m_texture;
  int m_width;
  int m_height;
  Sampler m_sampler;
  Vector m_texel_scale;

private:
  SDLTexture(const SDLTexture&) = delete;
  SDLTexture& operator=(const SDLTexture&) = delete;
};

#endif

/* EOF */
