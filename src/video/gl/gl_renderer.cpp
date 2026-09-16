// src/video/gl/gl_renderer.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//	Updated by GiBy 2013 for SDL2 <giby_the_kid@yahoo.fr>
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

#include "video/gl/gl_renderer.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include "SDL.h"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_request.hpp"
#include "video/gl/gl_painter.hpp"
#include "video/gl/gl_surface_data.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/util.hpp"

#ifdef USE_GLBINDING
#  include <glbinding/Binding.h>
#  include <glbinding/ContextInfo.h>
#  include <glbinding/gl/extension.h>
#  include <glbinding/callbacks.h>
#endif

#define LIGHTMAP_DIV 5

GLRenderer::GLRenderer() :
  m_window(),
  m_glcontext(),
  m_viewport(),
  m_fullscreen_active(false)
{
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  5);

  apply_video_mode();

  /* The swap interval belongs to a context, so asking for it before one
     exists is refused. This has to come after the window is up. */
  if(SDL_GL_SetSwapInterval(g_config->vsync) != 0)
  {
    log_info << "no support for vsync mode " << g_config->vsync << ": "
             << SDL_GetError() << std::endl;

    /* A driver that will not let a late frame through can usually still
       wait for every one, which is the nearest thing to what was asked. */
    if(g_config->vsync == -1 && SDL_GL_SetSwapInterval(1) == 0)
    {
      g_config->vsync = 1;
    }
  }

#ifdef USE_GLBINDING

  glbinding::Binding::initialize();

#ifdef USE_GLBINDING_DEBUG_OUTPUT
  glbinding::setCallbackMask(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue);

  glbinding::setAfterCallback([](const glbinding::FunctionCall & call) {
    std::cout << call.function.name() << "(";

    for (unsigned i = 0; i < call.parameters.size(); ++i)
    {
      std::cout << call.parameters[i]->asString();
      if (i < call.parameters.size() - 1)
        std::cout << ", ";
    }

      std::cout << ")";

    if (call.returnValue)
    {
      std::cout << " -> " << call.returnValue->asString();
    }

    std::cout << std::endl;
  });
#endif

  static auto extensions = glbinding::ContextInfo::extensions();
  log_info << "Using glbinding" << std::endl;
  log_info << "ARB_texture_non_power_of_two: " << static_cast<int>(extensions.find(GLextension::GL_ARB_texture_non_power_of_two) != extensions.end()) << std::endl;

#endif

  // setup opengl state and transform
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Init the projection matrix, viewport and stuff
  apply_config();

#ifdef HAVE_GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::ostringstream out;
    out << "GLRenderer: " << glewGetErrorString(err);
    throw std::runtime_error(out.str());
  }
  log_info << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  log_info << "GLEW_ARB_texture_non_power_of_two: " << static_cast<int>(GLEW_ARB_texture_non_power_of_two) << std::endl;
#elif !defined(USE_GLBINDING)
  log_info << "Using OpenGL with no extension loader" << std::endl;
#endif
}

GLRenderer::~GLRenderer()
{
  SDL_GL_DeleteContext(m_glcontext);
  SDL_DestroyWindow(m_window);
}

void
GLRenderer::flip()
{
  assert_gl("drawing");
  SDL_GL_SwapWindow(m_window);
}

void
GLRenderer::resize(int w, int h)
{
  /* Neither fullscreen nor maximised is a size anybody chose, so neither is
     kept. */
  if (!g_config->use_fullscreen && !g_config->window_maximised)
  {
    g_config->window_size = Size(w, h);
  }

  apply_config();
}

void
GLRenderer::apply_config()
{
  apply_video_mode();

  /* Ask how big the window came out rather than working it back out from
     the settings, since a window manager is free to hand back something
     other than what was asked for. */
  Size target_size;
  SDL_GL_GetDrawableSize(m_window, &target_size.width, &target_size.height);

  /* Zero means take the shape of the screen the game is on. */
  float aspect_ratio = 0.0f;
  if (g_config->aspect_size != Size(0, 0))
  {
    aspect_ratio = static_cast<float>(g_config->aspect_size.width) /
                   static_cast<float>(g_config->aspect_size.height);
  }

  Vector scale;
  Size logical_size;
  calculate_viewport(target_size,
                     aspect_ratio,
                     g_config->magnification,
                     scale,
                     logical_size,
                     m_viewport);

  SCREEN_WIDTH = logical_size.width;
  SCREEN_HEIGHT = logical_size.height;

  bool clear_buffers = (m_viewport.x != 0 || m_viewport.y != 0);
#ifdef __wii__
  /* OpenGX hands back whatever was in video memory, not a blank buffer. */
  clear_buffers = true;
#endif

  if (clear_buffers)
  {
    // Clear both buffers so that we get a clean black border without junk
    /* The lightmap leaves the clear colour set to its ambient one. */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(m_window);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(m_window);
  }

  glViewport(m_viewport.x, m_viewport.y, m_viewport.w, m_viewport.h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, 0);
  check_gl_error("Setting up view matrices");
}

void
GLRenderer::apply_video_mode()
{
  if (m_window)
  {
    if (!g_config->use_fullscreen)
    {
      SDL_SetWindowFullscreen(m_window, 0);

      /* Maximising a window that already is waits a full second and times out,
         and asking for a size it already has is wasted too. */
      const bool already_maximised =
        (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MAXIMIZED) != 0;

      if (g_config->window_maximised)
      {
        if (!already_maximised)
        {
          SDL_MaximizeWindow(m_window);
        }
      }
      else
      {
        if (already_maximised)
        {
          SDL_RestoreWindow(m_window);
        }

        /* Ask for the size the window is meant to be whenever it is not already
           it. Dragging the window to a new size records that size, so this asks
           for nothing in that case. */
        Size current_size;
        SDL_GetWindowSize(m_window, &current_size.width, &current_size.height);
        if (current_size != g_config->window_size)
        {
          SDL_SetWindowSize(m_window,
                            g_config->window_size.width,
                            g_config->window_size.height);
        }
      }
    }
    else
    {
      if (g_config->fullscreen_size.width == 0 &&
          g_config->fullscreen_size.height == 0)
      {
        if (SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
        {
          log_warning << "failed to switch to desktop fullscreen mode: "
                      << SDL_GetError() << std::endl;
        }
        else
        {
          log_info << "switched to desktop fullscreen mode" << std::endl;
        }
      }
      else
      {
        SDL_DisplayMode mode;
        mode.format = SDL_PIXELFORMAT_RGB888;
        mode.w = g_config->fullscreen_size.width;
        mode.h = g_config->fullscreen_size.height;
        mode.refresh_rate = g_config->fullscreen_refresh_rate;
        mode.driverdata = 0;

        if (SDL_SetWindowDisplayMode(m_window, &mode) != 0)
        {
          log_warning << "failed to set display mode: "
                      << mode.w << "x" << mode.h << "@" << mode.refresh_rate << ": "
                      << SDL_GetError() << std::endl;
        }
        else
        {
          if (SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN) != 0)
          {
            log_warning << "failed to switch to fullscreen mode: "
                        << mode.w << "x" << mode.h << "@" << mode.refresh_rate << ": "
                        << SDL_GetError() << std::endl;
          }
          else
          {
            log_info << "switched to fullscreen mode: "
                     << mode.w << "x" << mode.h << "@" << mode.refresh_rate << std::endl;
          }
        }
      }
    }
  }
  else
  {
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    Size size;


    if (g_config->use_fullscreen)
    {
      if (g_config->fullscreen_size == Size(0, 0))
      {
        /* This mode covers the screen whatever size is asked for, so the
           window is made at the size it should go back to when fullscreen is
           turned off. That is the size SDL remembers for it. */
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        size = g_config->window_size;
      }
      else
      {
        flags |= SDL_WINDOW_FULLSCREEN;
        size.width  = g_config->fullscreen_size.width;
        size.height = g_config->fullscreen_size.height;
      }
    }
    else
    {
      size = g_config->window_size;
    }

    m_window = SDL_CreateWindow("SuperTux",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              size.width, size.height,
                              flags);
    if (!m_window)
    {
      std::ostringstream msg;
      msg << "Couldn't set video mode " << size.width << "x" << size.height << ": " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
    else
    {
      m_glcontext = SDL_GL_CreateContext(m_window);
      if (!m_glcontext)
      {
        std::ostringstream msg;
        msg << "Couldn't create OpenGL context: " << SDL_GetError();
        throw std::runtime_error(msg.str());
      }

      SCREEN_WIDTH = size.width;
      SCREEN_HEIGHT = size.height;

      m_fullscreen_active = g_config->use_fullscreen;
    }
  }
}

void
GLRenderer::start_draw()
{
}

void
GLRenderer::end_draw()
{
}

void
GLRenderer::draw_surface(const DrawingRequest& request)
{
  GLPainter::draw_surface(request);
}

void
GLRenderer::draw_surface_part(const DrawingRequest& request)
{
  GLPainter::draw_surface_part(request);
}

void
GLRenderer::draw_gradient(const DrawingRequest& request)
{
  GLPainter::draw_gradient(request);
}

void
GLRenderer::draw_filled_rect(const DrawingRequest& request)
{
  GLPainter::draw_filled_rect(request);
}

void
GLRenderer::draw_inverse_ellipse(const DrawingRequest& request)
{
  GLPainter::draw_inverse_ellipse(request);
}

void
GLRenderer::draw_line(const DrawingRequest& request)
{
  GLPainter::draw_line(request);
}

void
GLRenderer::draw_triangle(const DrawingRequest& request)
{
  GLPainter::draw_triangle(request);
}

Vector
GLRenderer::to_logical(int physical_x, int physical_y) const
{
  return Vector(static_cast<float>(physical_x - m_viewport.x) * SCREEN_WIDTH / m_viewport.w,
                static_cast<float>(physical_y - m_viewport.y) * SCREEN_HEIGHT / m_viewport.h);
}

void
GLRenderer::warp_pointer(const Vector& logical)
{
  /* Rounded, not truncated: truncation always loses the fraction in the same
     direction, so a pointer put back over and over creeps one way. */
  const int x = static_cast<int>(std::lround(logical.x * m_viewport.w / SCREEN_WIDTH)) + m_viewport.x;
  const int y = static_cast<int>(std::lround(logical.y * m_viewport.h / SCREEN_HEIGHT)) + m_viewport.y;
  SDL_WarpMouseInWindow(m_window, x, y);
}

void
GLRenderer::set_gamma(float gamma)
{
  Uint16 ramp[256];
  SDL_CalculateGammaRamp(gamma, ramp);
  SDL_SetWindowGammaRamp(m_window, ramp, ramp, ramp);
}

/* EOF */
