// src/video/sdl/sdl_video_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_VIDEO_SYSTEM_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_VIDEO_SYSTEM_HPP

#include <memory>
#include <SDL.h>

#include "video/video_system.hpp"

class TextureManager;

class SDLVideoSystem : public VideoSystem
{
private:
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Lightmap> m_lightmap;
  std::unique_ptr<TextureManager> m_texture_manager;

public:
  SDLVideoSystem();

  Renderer& get_renderer() const override;
  Lightmap& get_lightmap() const override;
  TexturePtr new_texture(SDL_Surface *image) override;
  SurfaceData* new_surface_data(const Surface& surface) override;
  void free_surface_data(SurfaceData* surface_data) override;

  void apply_config() override;
  void resize(int w, int h) override;

private:
  SDLVideoSystem(const SDLVideoSystem&) = delete;
  SDLVideoSystem& operator=(const SDLVideoSystem&) = delete;
};

#endif // HEADER_SUPERTUX_VIDEO_SDL_SDL_VIDEO_SYSTEM_HPP

// EOF
