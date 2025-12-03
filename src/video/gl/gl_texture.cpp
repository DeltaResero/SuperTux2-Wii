// src/video/gl/gl_texture.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/gameconfig.hpp"
#include "video/gl/gl_texture.hpp"
#include "util/log.hpp"

#ifdef USE_GLBINDING
  #include <glbinding/ContextInfo.h>
#endif

namespace {

#ifdef GL_VERSION_ES_CM_1_0
inline bool is_power_of_2(int v)
{
  return (v & (v-1)) == 0;
}
#endif

inline int next_power_of_two(int val)
{
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

/**
 * Checks if NPOT textures are supported at runtime.
 * Returns true if the hardware/driver supports NPOT, false otherwise.
 */
inline bool has_npot_support()
{
#ifdef HAVE_NPOT_TEXTURES
  // NPOT is enabled in build system - now check if hardware supports it
  #ifdef USE_GLBINDING
    static auto extensions = glbinding::ContextInfo::extensions();
    return (extensions.find(GLextension::GL_ARB_texture_non_power_of_two) != extensions.end());
  #else
    #if !defined(GL_VERSION_ES_CM_1_0)
      // Desktop OpenGL with GLEW - check the extension
      return GLEW_ARB_texture_non_power_of_two;
    #else
      // OpenGL ES - typically no NPOT support in 1.x
      return false;
    #endif
  #endif
#else
  // NPOT disabled at build time
  return false;
#endif
}

} // namespace

GLTexture::GLTexture(unsigned int width, unsigned int height) :
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height()
{
#ifdef GL_VERSION_ES_CM_1_0
  assert(is_power_of_2(width));
  assert(is_power_of_2(height));
#endif
  m_texture_width  = width;
  m_texture_height = height;
  m_image_width  = width;
  m_image_height = height;

  assert_gl("before creating texture");
  glGenTextures(1, &m_handle);

  try {
    glBindTexture(GL_TEXTURE_2D, m_handle);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA), m_texture_width,
                 m_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

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
  m_image_width  = image->w;
  m_image_height = image->h;

  // Determine texture dimensions based on NPOT support
  if (has_npot_support())
  {
    // Hardware supports NPOT - use exact dimensions
    m_texture_width  = image->w;
    m_texture_height = image->h;
  }
  else
  {
    // No NPOT support - round up to power of two
    m_texture_width = next_power_of_two(image->w);
    m_texture_height = next_power_of_two(image->h);
  }

  // Check for excessively large textures (sanity check)
  if (m_texture_width > 4096 || m_texture_height > 4096)
  {
    log_warning << "Texture " << image->w << "x" << image->h
                << " would require " << m_texture_width << "x" << m_texture_height
                << " allocation (unusually large!)" << std::endl;
  }

  // Create POT surface if needed
  SDL_Surface* surface_to_upload = image;
  SDL_Surface* pot_surface = nullptr;

  if (m_texture_width != image->w || m_texture_height != image->h)
  {
    // We need POT texture - create padded surface
    // This ensures proper rendering on platforms without NPOT support

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    pot_surface = SDL_CreateRGBSurface(0,
                                       m_texture_width, m_texture_height, 32,
                                       0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    pot_surface = SDL_CreateRGBSurface(0,
                                       m_texture_width, m_texture_height, 32,
                                       0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

    if (!pot_surface)
    {
      throw std::runtime_error("Couldn't create POT surface: out of memory");
    }

    // Clear surface to transparent black to avoid garbage in padding area
    Uint32 clear_color = SDL_MapRGBA(pot_surface->format, 0, 0, 0, 0);
    SDL_FillRect(pot_surface, nullptr, clear_color);

    // Disable blending for clean pixel copy
    SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);

    // Blit original image into top-left corner of POT surface
    if (SDL_BlitSurface(image, nullptr, pot_surface, nullptr) != 0)
    {
      SDL_FreeSurface(pot_surface);
      std::ostringstream msg;
      msg << "Failed to blit surface: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }

    // Edge Smearing (Guttering)
    // Copy the last row and column into the padding area.
    // This allows GL_LINEAR filtering to work without blending with transparent padding pixels,
    // which eliminates visible seams between tiles.

    SDL_Rect src_rect;
    SDL_Rect dst_rect;

    // Smear Right Edge
    src_rect.x = image->w - 1;
    src_rect.y = 0;
    src_rect.w = 1;
    src_rect.h = image->h;
    dst_rect.x = image->w;
    dst_rect.y = 0;
    SDL_BlitSurface(image, &src_rect, pot_surface, &dst_rect);

    // Smear Bottom Edge
    src_rect.x = 0;
    src_rect.y = image->h - 1;
    src_rect.w = image->w;
    src_rect.h = 1;
    dst_rect.x = 0;
    dst_rect.y = image->h;
    SDL_BlitSurface(image, &src_rect, pot_surface, &dst_rect);

    // Smear Bottom-Right Corner Pixel
    src_rect.x = image->w - 1;
    src_rect.y = image->h - 1;
    src_rect.w = 1;
    src_rect.h = 1;
    dst_rect.x = image->w;
    dst_rect.y = image->h;
    SDL_BlitSurface(image, &src_rect, pot_surface, &dst_rect);

    surface_to_upload = pot_surface;

    // Log POT conversion for debugging
    int padding_kb = ((m_texture_width * m_texture_height) - (image->w * image->h)) * 4 / 1024;
    log_debug << "Texture " << image->w << "x" << image->h
              << " padded to " << m_texture_width << "x" << m_texture_height
              << " (+" << padding_kb << " KB padding)" << std::endl;
  }

  // Now create the format-converted surface for OpenGL upload
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* convert = SDL_CreateRGBSurface(0,
                                              m_texture_width, m_texture_height, 32,
                                              0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* convert = SDL_CreateRGBSurface(0,
                                              m_texture_width, m_texture_height, 32,
                                              0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  if(convert == nullptr)
  {
    if (pot_surface)
    {
      SDL_FreeSurface(pot_surface);
    }
    throw std::runtime_error("Couldn't create texture: out of memory");
  }

  SDL_SetSurfaceBlendMode(surface_to_upload, SDL_BLENDMODE_NONE);
  SDL_BlitSurface(surface_to_upload, nullptr, convert, nullptr);

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
                 m_texture_width, m_texture_height, 0, sdl_format,
                 GL_UNSIGNED_BYTE, convert->pixels);

    // Don't use mipmaps
    if(false)
    {
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    if(SDL_MUSTLOCK(convert))
    {
      SDL_UnlockSurface(convert);
    }

    assert_gl("creating texture");

    set_texture_params();
  } catch(...) {
    glDeleteTextures(1, &m_handle);
    SDL_FreeSurface(convert);
    if (pot_surface)
    {
      SDL_FreeSurface(pot_surface);
    }
    throw;
  }

  SDL_FreeSurface(convert);
  if (pot_surface)
  {
    SDL_FreeSurface(pot_surface);
  }
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

// EOF
