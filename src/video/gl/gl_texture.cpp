// src/video/gl/gl_texture.cpp
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

#include <config.h>
#include <stdexcept>

#include "supertux/gameconfig.hpp"
#include "video/gl/gl_texture.hpp"

#ifdef USE_GLBINDING
  #include <glbinding/ContextInfo.h>
#endif

namespace {

inline unsigned int next_power_of_two(unsigned int val)
{
  unsigned int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

/** Round up to a whole number of whatever the hardware stores textures in.
    With an alignment of one this hands back the size it was given. */
inline unsigned int align_up(unsigned int val, unsigned int alignment)
{
  return (val + alignment - 1) / alignment * alignment;
}

} // namespace

GLTexture::GLTexture(unsigned int width, unsigned int height) :
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height()
{
  m_texture_width  = width;
  m_texture_height = height;
  m_image_width  = width;
  m_image_height = height;

  assert_gl("before creating texture");
  glGenTextures(1, &m_handle);

  try {
    glBindTexture(GL_TEXTURE_2D, m_handle);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA),
                 static_cast<GLsizei>(m_texture_width),
                 static_cast<GLsizei>(m_texture_height),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    set_texture_params();
  } catch(...) {
    glDeleteTextures(1, &m_handle);
    throw;
  }
}

GLTexture::GLTexture(SDL_Surface* image) :
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height()
{
  /* Whether the driver will take a texture whose sides are not powers of
     two. It is an extension, so answering needs something to load it with.
     A build with no loader either has been told the answer or falls back on
     the older rule, which costs roughly twice the memory. */
#ifdef USE_GLBINDING
  static auto extensions = glbinding::ContextInfo::extensions();
  const bool npot = extensions.find(GLextension::GL_ARB_texture_non_power_of_two) != extensions.end();
#elif defined(HAVE_GLEW)
  const bool npot = GLEW_ARB_texture_non_power_of_two != 0;
#elif defined(HAVE_NPOT_TEXTURES)
  const bool npot = true;
#else
  const bool npot = false;
#endif

  /* SDL states a surface's size as a signed number, though a surface never
     has a negative side. Cross over once here rather than at every use. */
  const unsigned int image_width  = static_cast<unsigned int>(image->w);
  const unsigned int image_height = static_cast<unsigned int>(image->h);

  if (npot)
  {
    m_texture_width  = align_up(image_width, TEXTURE_ALIGNMENT);
    m_texture_height = align_up(image_height, TEXTURE_ALIGNMENT);
  }
  else
  {
    m_texture_width  = next_power_of_two(image_width);
    m_texture_height = next_power_of_two(image_height);
  }

  m_image_width  = image_width;
  m_image_height = image_height;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* convert = SDL_CreateRGBSurface(0,
                                              static_cast<int>(m_texture_width),
                                              static_cast<int>(m_texture_height), 32,
                                              0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* convert = SDL_CreateRGBSurface(0,
                                              static_cast<int>(m_texture_width),
                                              static_cast<int>(m_texture_height), 32,
                                              0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  if(convert == 0) {
    throw std::runtime_error("Couldn't create texture: out of memory");
  }

  SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);
  SDL_BlitSurface(image, 0, convert, 0);

  /* Whatever the image does not cover is left transparent, and GL_LINEAR
     mixes those empty pixels in every time it samples the last row or
     column, which shows up as a faded line down the edge of the sprite.
     Repeating the edge one pixel into the padding gives the filter the
     image's own colour to blend with instead of a hole. One pixel is
     enough, because the texture coordinates stop at the image and the rest
     of the padding is never sampled. */
  const bool pad_right  = static_cast<unsigned int>(image->w) < m_texture_width;
  const bool pad_bottom = static_cast<unsigned int>(image->h) < m_texture_height;

  if(pad_right)
  {
    SDL_Rect src = { image->w - 1, 0, 1, image->h };
    SDL_Rect dst = { image->w,     0, 1, image->h };
    SDL_BlitSurface(image, &src, convert, &dst);
  }

  if(pad_bottom)
  {
    SDL_Rect src = { 0, image->h - 1, image->w, 1 };
    SDL_Rect dst = { 0, image->h,     image->w, 1 };
    SDL_BlitSurface(image, &src, convert, &dst);
  }

  if(pad_right && pad_bottom)
  {
    SDL_Rect src = { image->w - 1, image->h - 1, 1, 1 };
    SDL_Rect dst = { image->w,     image->h,     1, 1 };
    SDL_BlitSurface(image, &src, convert, &dst);
  }

  assert_gl("before creating texture");
  glGenTextures(1, &m_handle);

  try {
    GLenum sdl_format;
    if(convert->format->BytesPerPixel == 3)
      sdl_format = GL_RGB;
    else if(convert->format->BytesPerPixel == 4)
      sdl_format = GL_RGBA;
    else {
      sdl_format = GL_RGBA;
      assert(false);
    }

    glBindTexture(GL_TEXTURE_2D, m_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#if defined(GL_UNPACK_ROW_LENGTH) || defined(USE_GLBINDING)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, convert->pitch/convert->format->BytesPerPixel);
#else
    /* OpenGL ES doesn't support UNPACK_ROW_LENGTH, let's hope SDL didn't add
     * padding bytes, otherwise we need some extra code here... */
    assert(convert->pitch == m_texture_width * convert->format->BytesPerPixel);
#endif

    if(SDL_MUSTLOCK(convert))
    {
      SDL_LockSurface(convert);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA),
                 static_cast<GLsizei>(m_texture_width),
                 static_cast<GLsizei>(m_texture_height),
                 0, sdl_format, GL_UNSIGNED_BYTE, convert->pixels);

    if(SDL_MUSTLOCK(convert))
    {
      SDL_UnlockSurface(convert);
    }

    assert_gl("creating texture");

    set_texture_params();
  } catch(...) {
    glDeleteTextures(1, &m_handle);
    SDL_FreeSurface(convert);
    throw;
  }
  SDL_FreeSurface(convert);
}

GLTexture::~GLTexture()
{
  glDeleteTextures(1, &m_handle);
}

void
GLTexture::set_texture_params()
{
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

  assert_gl("set texture params");
}

/* EOF */
