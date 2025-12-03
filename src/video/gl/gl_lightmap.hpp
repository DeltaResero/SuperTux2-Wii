// src/video/gl/gl_lightmap.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_GL_LIGHTMAP_HPP
#define HEADER_SUPERTUX_VIDEO_GL_LIGHTMAP_HPP

#include <config.h>
#include "video/lightmap.hpp"
#include "video/color.hpp"

#include <memory>

struct DrawingRequest;
class Texture;
class GLTexture;

class GLLightmap : public Lightmap
{
public:
  GLLightmap();
  ~GLLightmap();

  void start_draw(const Color &ambient_color) override;
  void end_draw() override;
  void do_draw() override;
  void draw_surface(const DrawingRequest& request) override;
  void draw_surface_part(const DrawingRequest& request) override;
  void draw_gradient(const DrawingRequest& request) override;
  void draw_filled_rect(const DrawingRequest& request) override;
  void draw_inverse_ellipse(const DrawingRequest& request) override;
  void get_light(const DrawingRequest& request) const override;
  void draw_line(const DrawingRequest& request) override;
  void draw_triangle(const DrawingRequest& request) override;

private:
  static const int s_LIGHTMAP_DIV = 5;

  std::shared_ptr<GLTexture> m_lightmap;
  int m_lightmap_width;
  int m_lightmap_height;
  float m_lightmap_uv_right;
  float m_lightmap_uv_bottom;
  GLfloat m_old_viewport[4]; //holds viewport before redefining in start_draw - returned from glGet
  Color m_ambient_color;

#ifdef USE_ROUNDTRIP_LIGHTMAP
  // Buffer for round-trip lightmap mode (glReadPixels + glTexSubImage2D)
  // Used on platforms where glCopyTexSubImage2D is not available (e.g., OpenGX/Wii)
  GLubyte* m_lightmap_buffer;
#endif

private:
  GLLightmap(const GLLightmap&);
  GLLightmap& operator=(const GLLightmap&);
};

#endif // HEADER_SUPERTUX_VIDEO_GL_LIGHTMAP_HPP

// EOF
