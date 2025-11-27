// src/video/gl/gl_texture.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_GL_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_GL_TEXTURE_HPP

#include "video/texture.hpp"

/**
 * This class is a wrapper around a texture handle. It stores the texture width
 * and height and provides convenience functions for uploading SDL_Surfaces
 * into the texture
 */
class GLTexture : public Texture
{
protected:
  GLuint m_handle;
  unsigned int m_texture_width;
  unsigned int m_texture_height;
  unsigned int m_image_width;
  unsigned int m_image_height;

public:
  GLTexture(unsigned int width, unsigned int height);
  GLTexture(SDL_Surface* image);
  ~GLTexture();

  const GLuint &get_handle() const {
    return m_handle;
  }

  void set_handle(GLuint handle) {
    m_handle = handle;
  }

  unsigned int get_texture_width() const
  {
    return m_texture_width;
  }

  unsigned int get_texture_height() const
  {
    return m_texture_height;
  }

  unsigned int get_image_width() const
  {
    return m_image_width;
  }

  unsigned int get_image_height() const
  {
    return m_image_height;
  }

  void set_image_width(unsigned int width)
  {
    m_image_width = width;
  }

  void set_image_height(unsigned int height)
  {
    m_image_height = height;
  }

private:
  void set_texture_params();
};

#endif // HEADER_SUPERTUX_VIDEO_GL_TEXTURE_HPP

// EOF
