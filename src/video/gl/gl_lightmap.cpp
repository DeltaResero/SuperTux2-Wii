// src/video/gl/gl_lightmap.cpp
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

#include "video/gl/gl_lightmap.hpp"

#include <SDL_image.h>
#include <algorithm>
#include <assert.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdexcept>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"
#include "video/drawing_request.hpp"
#include "video/font.hpp"
#include "video/gl/gl_painter.hpp"
#include "video/gl/gl_renderer.hpp"
#include "video/gl/gl_surface_data.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/glutil.hpp"
#include "video/lightmap.hpp"
#include "video/renderer.hpp"
#include "video/surface.hpp"

inline unsigned int next_po2(unsigned int val)
{
  unsigned int result = 1;
  while(result < val)
    result *= 2;

  return result;
}

GLLightmap::GLLightmap() :
  m_lightmap(),
  m_lightmap_width(),
  m_lightmap_height(),
  m_lightmap_uv_right(),
  m_lightmap_uv_bottom(),
  m_old_viewport(),
  m_readback(),
  m_readback_filled(false)
#ifdef ENABLE_LIGHTMAP_FBO
  , m_framebuffer()
#endif
{
  m_lightmap_width = SCREEN_WIDTH / s_LIGHTMAP_DIV;
  m_lightmap_height = SCREEN_HEIGHT / s_LIGHTMAP_DIV;
  unsigned int width = next_po2(static_cast<unsigned int>(m_lightmap_width));
  unsigned int height = next_po2(static_cast<unsigned int>(m_lightmap_height));

  m_lightmap.reset(new GLTexture(width, height));

  m_lightmap_uv_right = static_cast<float>(m_lightmap_width) / static_cast<float>(width);
  m_lightmap_uv_bottom = static_cast<float>(m_lightmap_height) / static_cast<float>(height);

  m_readback.resize(static_cast<size_t>(m_lightmap_width * m_lightmap_height * 4));

#ifdef ENABLE_LIGHTMAP_FBO
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, m_lightmap->get_handle(), 0);

  /* Ask before drawing anything, since a framebuffer the driver will not
     accept renders nothing at all and reports no error while doing it. */
  const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if(status != GL_FRAMEBUFFER_COMPLETE)
  {
    glDeleteFramebuffers(1, &m_framebuffer);
    std::ostringstream msg;
    msg << "Couldn't hang the lightmap on a framebuffer: status " << status;
    throw std::runtime_error(msg.str());
  }

  /* A texture made with no data starts undefined, and the rounded-up edges
     outside the lightmap are never drawn over. */
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

GLLightmap::~GLLightmap()
{
#ifdef ENABLE_LIGHTMAP_FBO
  glDeleteFramebuffers(1, &m_framebuffer);
#endif
}

#ifdef ENABLE_LIGHTMAP_FBO

void
GLLightmap::bind_lightmap()
{
  glGetIntegerv(GL_VIEWPORT, m_old_viewport); //save viewport
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  /* The texture is larger than the lightmap wherever its sides had to be
     rounded up, so draw into the corner the texture coordinates read from. */
  glViewport(0, 0, m_lightmap_width, m_lightmap_height);
}

void
GLLightmap::unbind_lightmap()
{
  /* The lights were drawn into the texture as they happened, so there is
     nothing to copy. Hand the screen back and restore the viewport. */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(m_old_viewport[0], m_old_viewport[1],
             m_old_viewport[2], m_old_viewport[3]);
}

#else

void
GLLightmap::bind_lightmap()
{
  glGetIntegerv(GL_VIEWPORT, m_old_viewport); //save viewport
  glViewport(m_old_viewport[0],
             m_old_viewport[3] - m_lightmap_height + m_old_viewport[1],
             m_lightmap_width, m_lightmap_height);
}

void
GLLightmap::unbind_lightmap()
{
  /* The lights went to a corner of the screen, so lift that corner into the
     texture before the scene is drawn over the top of it. */
  glDisable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, m_lightmap->get_handle());
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                      m_old_viewport[0],
                      m_old_viewport[3] - m_lightmap_height + m_old_viewport[1],
                      m_lightmap_width, m_lightmap_height);

  glViewport(m_old_viewport[0], m_old_viewport[1],
             m_old_viewport[2], m_old_viewport[3]);
  glEnable(GL_BLEND);
}

#endif

void
GLLightmap::start_draw(const Color &ambient_color)
{
  m_readback_filled = false;

  bind_lightmap();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor( ambient_color.red, ambient_color.green, ambient_color.blue, 1 );
  glClear(GL_COLOR_BUFFER_BIT);
}

void
GLLightmap::end_draw()
{
  unbind_lightmap();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0, 0, 0, 1 );
  glClear(GL_COLOR_BUFFER_BIT);
}

void
GLLightmap::do_draw()
{
  // multiple the lightmap with the framebuffer
  glBlendFunc(GL_DST_COLOR, GL_ZERO);

  glBindTexture(GL_TEXTURE_2D, m_lightmap->get_handle());

  float vertices[] = {
    0, 0,
    float(SCREEN_WIDTH), 0,
    float(SCREEN_WIDTH), float(SCREEN_HEIGHT),
    0, float(SCREEN_HEIGHT)
  };
  glVertexPointer(2, GL_FLOAT, 0, vertices);

  float uvs[] = {
    0,                 m_lightmap_uv_bottom,
    m_lightmap_uv_right, m_lightmap_uv_bottom,
    m_lightmap_uv_right, 0,
    0, 0
  };
  glTexCoordPointer(2, GL_FLOAT, 0, uvs);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
GLLightmap::draw_surface(const DrawingRequest& request)
{
  /* Fractions kept, as SDLPainter does: a whole pixel here is five on screen. */
  GLPainter::draw_surface(request, Vector());
}

void
GLLightmap::draw_surface_part(const DrawingRequest& request)
{
  GLPainter::draw_surface_part(request, Vector());
}

void
GLLightmap::draw_gradient(const DrawingRequest& request)
{
  GLPainter::draw_gradient(request);
}

void
GLLightmap::draw_filled_rect(const DrawingRequest& request)
{
  GLPainter::draw_filled_rect(request);
}

void
GLLightmap::draw_inverse_ellipse(const DrawingRequest& request)
{
  GLPainter::draw_inverse_ellipse(request);
}

void
GLLightmap::draw_line(const DrawingRequest& request)
{
  GLPainter::draw_line(request);
}

void
GLLightmap::draw_triangle(const DrawingRequest& request)
{
  GLPainter::draw_triangle(request);
}

void
GLLightmap::read_back() const
{
  /* A framebuffer of its own holds the lightmap at the origin, the screen
     holds it in the top left corner of the viewport. */
#ifdef ENABLE_LIGHTMAP_FBO
  const GLint origin_x = 0;
  const GLint origin_y = 0;
#else
  const GLint origin_x = m_old_viewport[0];
  const GLint origin_y = m_old_viewport[1] + m_old_viewport[3] - m_lightmap_height;
#endif

  /* Four bytes a pixel so a row never needs its alignment stated. */
  glReadPixels(origin_x, origin_y, m_lightmap_width, m_lightmap_height,
               GL_RGBA, GL_UNSIGNED_BYTE, m_readback.data());

  m_readback_filled = true;
}

void
GLLightmap::get_light(const DrawingRequest& request) const
{
  const GetLightRequest* getlightrequest
    = static_cast<GetLightRequest*>(request.request_data);

  /* These requests carry the last layer, so every light is already drawn. */
  if(!m_readback_filled)
    read_back();

  /* A position at the very top or the far right divides to one past the last
     row or column. */
  const int x = std::min(static_cast<int>(request.pos.x * m_lightmap_width / SCREEN_WIDTH),
                         m_lightmap_width - 1);
  const int y = std::min(static_cast<int>((SCREEN_HEIGHT - request.pos.y) * m_lightmap_height / SCREEN_HEIGHT),
                         m_lightmap_height - 1);

  const GLubyte* pixel
    = &m_readback[static_cast<size_t>((y * m_lightmap_width + x) * 4)];

  *(getlightrequest->color_ptr) = Color(pixel[0] / 255.0f,
                                        pixel[1] / 255.0f,
                                        pixel[2] / 255.0f);
}

/* EOF */
