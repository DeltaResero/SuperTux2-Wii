// src/video/sdl/sdl_painter.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_SDL_PAINTER_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_PAINTER_HPP

#include "SDL.h"
#include "video/renderer.hpp"

class SDLPainter
{
public:
  static void draw_surface(SDL_Renderer* renderer, const DrawingRequest& request);
  static void draw_surface_part(SDL_Renderer* renderer, const DrawingRequest& request);
  static void draw_gradient(SDL_Renderer* renderer, const DrawingRequest& request);
  static void draw_filled_rect(SDL_Renderer* renderer, const DrawingRequest& request);
  static void draw_inverse_ellipse(SDL_Renderer* renderer, const DrawingRequest& request);
  static void draw_line(SDL_Renderer* renderer, const DrawingRequest& request);
  static void draw_triangle(SDL_Renderer* renderer, const DrawingRequest& request);

private:
  SDLPainter(const SDLPainter&);
  SDLPainter& operator=(const SDLPainter&);
};

#endif // HEADER_SUPERTUX_VIDEO_SDL_PAINTER_HPP

// EOF
