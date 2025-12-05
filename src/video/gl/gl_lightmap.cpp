// src/video/gl/gl_lightmap.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "video/gl/gl_lightmap.hpp"

#include <SDL_image.h>
#include <algorithm>
#include <assert.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/obstackpp.hpp"
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
#include "video/texture_manager.hpp"
#include "util/log.hpp"

inline int next_po2(int val)
{
  int result = 1;
  while(result < val)
    result *= 2;

  return result;
}

namespace {

/**
 * Determines the active lightmap mode based on config and build flags.
 * Returns true if lightmap rendering should be active.
 */
bool should_use_lightmap()
{
#ifdef HAVE_LIGHTMAP
  // Check user's config preference
  switch (g_config->lightmap_mode) {
    case LightmapMode::DISABLED:
      return false;

    case LightmapMode::NATIVE:
    case LightmapMode::ROUNDTRIP:
      return true;

    case LightmapMode::AUTO:
    default:
      // In AUTO mode, use the build system's default
      return true;
  }
#else
  // Lightmap disabled at build time
  return false;
#endif
}

} // namespace

GLLightmap::GLLightmap() :
  m_lightmap(),
  m_lightmap_width(),
  m_lightmap_height(),
  m_lightmap_uv_right(),
  m_lightmap_uv_bottom(),
  m_old_viewport(),
  m_ambient_color()
#ifdef USE_ROUNDTRIP_LIGHTMAP
  , m_lightmap_buffer(nullptr)
#endif
{
  m_lightmap_width = SCREEN_WIDTH / s_LIGHTMAP_DIV;
  m_lightmap_height = SCREEN_HEIGHT / s_LIGHTMAP_DIV;
  unsigned int width = next_po2(m_lightmap_width);
  unsigned int height = next_po2(m_lightmap_height);

  m_lightmap.reset(new GLTexture(width, height));

  m_lightmap_uv_right = static_cast<float>(m_lightmap_width) / static_cast<float>(width);
  m_lightmap_uv_bottom = static_cast<float>(m_lightmap_height) / static_cast<float>(height);
  TextureManager::current()->register_texture(m_lightmap.get());

  // CRITICAL FOR WII: Initialize lightmap texture to black
  // OpenGX doesn't guarantee zero-initialization of textures
  // Without this, you get garbage/noise in the lightmap
#if defined(__wii__) || defined(_WII_) || defined(__gamecube__)
  glBindTexture(GL_TEXTURE_2D, m_lightmap->get_handle());
  std::vector<GLubyte> black_pixels(width * height * 4, 0);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                  GL_RGBA, GL_UNSIGNED_BYTE, black_pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  log_debug << "Wii: Cleared lightmap texture to prevent garbage" << std::endl;
#endif

#ifdef USE_ROUNDTRIP_LIGHTMAP
  // Allocate buffer for round-trip mode
  // Size: width * height * 4 bytes (RGBA)
  // For typical 128x96 lightmap: ~49KB
  m_lightmap_buffer = new GLubyte[m_lightmap_width * m_lightmap_height * 4];
  log_info << "Lightmap: Using ROUNDTRIP mode (buffer size: "
           << (m_lightmap_width * m_lightmap_height * 4) / 1024 << " KB)" << std::endl;
#else
  log_info << "Lightmap: Using NATIVE mode" << std::endl;
#endif
}

GLLightmap::~GLLightmap()
{
#ifdef USE_ROUNDTRIP_LIGHTMAP
  delete[] m_lightmap_buffer;
  m_lightmap_buffer = nullptr;
#endif
}

void
GLLightmap::start_draw(const Color &ambient_color)
{
#ifdef HAVE_LIGHTMAP
  if (!should_use_lightmap()) {
    return; // Lightmap disabled, skip rendering
  }

  m_ambient_color = ambient_color;

  glGetFloatv(GL_VIEWPORT, m_old_viewport); //save viewport
  glViewport(m_old_viewport[0], m_old_viewport[3] - m_lightmap_height + m_old_viewport[1], m_lightmap_width, m_lightmap_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#ifdef GL_VERSION_ES_CM_1_0
  glOrthof(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
#else
  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
#endif
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor( ambient_color.red, ambient_color.green, ambient_color.blue, 1 );
  glClear(GL_COLOR_BUFFER_BIT);
#endif // HAVE_LIGHTMAP
}

void
GLLightmap::end_draw()
{
#ifdef HAVE_LIGHTMAP
  if (!should_use_lightmap()) {
    return; // Lightmap disabled, skip rendering
  }

  glDisable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, m_lightmap->get_handle());

#ifdef USE_ROUNDTRIP_LIGHTMAP
  // Round-trip mode: Read from framebuffer to CPU, then upload to texture
  // This is used on platforms where glCopyTexSubImage2D is not available (e.g., OpenGX/Wii)

  // Read pixels from framebuffer into CPU memory
  glReadPixels(m_old_viewport[0],
               m_old_viewport[3] - m_lightmap_height + m_old_viewport[1],
               m_lightmap_width, m_lightmap_height,
               GL_RGBA, GL_UNSIGNED_BYTE, m_lightmap_buffer);

  // Upload from CPU memory to texture
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                  m_lightmap_width, m_lightmap_height,
                  GL_RGBA, GL_UNSIGNED_BYTE, m_lightmap_buffer);
#else
  // Native mode: Direct VRAM-to-VRAM copy (fastest)
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                      m_old_viewport[0],
                      m_old_viewport[3] - m_lightmap_height + m_old_viewport[1],
                      m_lightmap_width, m_lightmap_height);
#endif

  glViewport(m_old_viewport[0], m_old_viewport[1], m_old_viewport[2], m_old_viewport[3]);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#ifdef GL_VERSION_ES_CM_1_0
  glOrthof(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
#else
  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
#endif
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_BLEND);

  glClearColor(0, 0, 0, 1 );
  glClear(GL_COLOR_BUFFER_BIT);
#endif // HAVE_LIGHTMAP
}

void
GLLightmap::do_draw()
{
#ifdef HAVE_LIGHTMAP
  if (!should_use_lightmap()) {
    return; // Lightmap disabled, skip rendering
  }

  // skip lightmap generation if ambient color is black
  if(m_ambient_color.red == 0 && m_ambient_color.green == 0 && m_ambient_color.blue == 0)
    return;

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
#endif // HAVE_LIGHTMAP
}

void
GLLightmap::draw_surface(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_surface(request);
  }
#endif
}

void
GLLightmap::draw_surface_part(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_surface_part(request);
  }
#endif
}

void
GLLightmap::draw_gradient(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_gradient(request);
  }
#endif
}

void
GLLightmap::draw_filled_rect(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_filled_rect(request);
  }
#endif
}

void
GLLightmap::draw_inverse_ellipse(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_inverse_ellipse(request);
  }
#endif
}

void
GLLightmap::draw_line(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_line(request);
  }
#endif
}

void
GLLightmap::draw_triangle(const DrawingRequest& request)
{
#ifdef HAVE_LIGHTMAP
  if (should_use_lightmap()) {
    GLPainter::draw_triangle(request);
  }
#endif
}

void
GLLightmap::get_light(const DrawingRequest& request) const
{
#ifdef HAVE_LIGHTMAP
  if (!should_use_lightmap()) {
    // Lightmap disabled - return full brightness
    const GetLightRequest* getlightrequest
      = static_cast<GetLightRequest*>(request.request_data);
    *(getlightrequest->color_ptr) = Color(1.0f, 1.0f, 1.0f);
    return;
  }

  const GetLightRequest* getlightrequest
    = static_cast<GetLightRequest*>(request.request_data);

  float pixels[3];
  for( int i = 0; i<3; i++)
    pixels[i] = 0.0f; //set to black

  float posX = request.pos.x * m_lightmap_width / SCREEN_WIDTH + m_old_viewport[0];
  float posY = m_old_viewport[3] + m_old_viewport[1] - request.pos.y * m_lightmap_height / SCREEN_HEIGHT;
  glReadPixels((GLint) posX, (GLint) posY , 1, 1, GL_RGB, GL_FLOAT, pixels);
  *(getlightrequest->color_ptr) = Color( pixels[0], pixels[1], pixels[2]);
#else
  // Lightmap not compiled in - return full brightness
  const GetLightRequest* getlightrequest
    = static_cast<GetLightRequest*>(request.request_data);
  *(getlightrequest->color_ptr) = Color(1.0f, 1.0f, 1.0f);
#endif // HAVE_LIGHTMAP
}

// EOF
