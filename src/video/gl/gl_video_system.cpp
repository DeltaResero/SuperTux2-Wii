// src/video/gl/gl_video_system.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/gl/gl_video_system.hpp"

#include <limits>

#include "util/log.hpp"
#include "video/gl/gl_lightmap.hpp"
#include "video/gl/gl_renderer.hpp"
#include "video/gl/gl_surface_data.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/glutil.hpp"
#include "video/texture_manager.hpp"

GLVideoSystem::GLVideoSystem() :
  m_texture_manager(new TextureManager),
  m_renderer(new GLRenderer),
  m_lightmap(new GLLightmap),
  m_max_texture_size(std::numeric_limits<unsigned int>::max())
{
  /* The renderer is built before this runs, so there is a context to ask.
     Every implementation answers this and the answer is never below 64, so
     nothing back means the question failed rather than that the ceiling is
     absent. */
  GLint size = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
  if(size > 0)
  {
    m_max_texture_size = static_cast<unsigned int>(size);
    log_info << "Max texture size: " << m_max_texture_size << std::endl;
  }
  else
  {
    log_warning << "Driver would not say how large a texture may be" << std::endl;
  }
}

Renderer&
GLVideoSystem::get_renderer() const
{
  return *m_renderer;
}

Lightmap&
GLVideoSystem::get_lightmap() const
{
  return *m_lightmap;
}

TexturePtr
GLVideoSystem::new_texture(SDL_Surface* image)
{
  return TexturePtr(new GLTexture(image));
}

unsigned int
GLVideoSystem::get_max_texture_size() const
{
  return m_max_texture_size;
}

SurfaceData*
GLVideoSystem::new_surface_data(const Surface& surface)
{
  return new GLSurfaceData(surface);
}

void
GLVideoSystem::free_surface_data(SurfaceData* surface_data)
{
  delete surface_data;
}

void
GLVideoSystem::apply_config()
{
  m_renderer->apply_config();
}

void
GLVideoSystem::resize(int w, int h)
{
  m_renderer->resize(w, h);
  m_lightmap.reset(new GLLightmap);
}

void
GLVideoSystem::set_vsync(int mode)
{
  if(SDL_GL_SetSwapInterval(mode) < 0)
  {
    log_warning << "Couldn't set vsync mode " << mode << ": "
                << SDL_GetError() << std::endl;
  }
}

int
GLVideoSystem::get_vsync() const
{
  return SDL_GL_GetSwapInterval();
}

/* EOF */
