// src/video/sdl/sdl_video_system.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "video/sdl/sdl_video_system.hpp"

#include "supertux/gameconfig.hpp"
#include "video/lightmap.hpp"
#include "video/renderer.hpp"
#include "video/sdl/sdl_lightmap.hpp"
#include "video/sdl/sdl_renderer.hpp"
#include "video/sdl/sdl_surface_data.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/texture.hpp"
#include "video/video_system.hpp"

SDLVideoSystem::SDLVideoSystem() :
  m_renderer(new SDLRenderer),
  m_lightmap(new SDLLightmap),
  m_texture_manager(new TextureManager)
{
}

Renderer&
SDLVideoSystem::get_renderer() const
{
  return *m_renderer;
}

Lightmap&
SDLVideoSystem::get_lightmap() const
{
  return *m_lightmap;
}

TexturePtr
SDLVideoSystem::new_texture(SDL_Surface* image)
{
  return TexturePtr(new SDLTexture(image));
}

SurfaceData*
SDLVideoSystem::new_surface_data(const Surface& surface)
{
  return new SDLSurfaceData(surface);
}

void
SDLVideoSystem::free_surface_data(SurfaceData* surface_data)
{
  delete surface_data;
}

void
SDLVideoSystem::apply_config()
{
  m_renderer->apply_config();
}

void
SDLVideoSystem::resize(int w, int h)
{
  m_renderer->resize(w, h);
  m_lightmap.reset(new SDLLightmap);
}

// EOF
