// src/video/renderer.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_RENDERER_HPP

#include <memory>
#include <string>
#include <vector>

#include <SDL_video.h>
#include <assert.h>
#include <stdint.h>
#include <obstack.h>

#include "math/rectf.hpp"
#include "math/vector.hpp"
#include "video/color.hpp"
#include "video/font.hpp"
#include "video/glutil.hpp"
#include "video/surface.hpp"

class Surface;
class Texture;
struct DrawingRequest;

class Renderer
{
public:
  Renderer();
  virtual ~Renderer();

  virtual void start_draw() = 0;
  virtual void end_draw() = 0;
  virtual void draw_surface(const DrawingRequest& request) = 0;
  virtual void draw_surface_part(const DrawingRequest& request) = 0;
  virtual void draw_gradient(const DrawingRequest& request) = 0;
  virtual void draw_filled_rect(const DrawingRequest& request)= 0;
  virtual void draw_inverse_ellipse(const DrawingRequest& request)= 0;
  virtual void draw_line(const DrawingRequest& request)= 0;
  virtual void draw_triangle(const DrawingRequest& request)= 0;
  virtual void do_take_screenshot() = 0;
  virtual void flip() = 0;
  virtual void resize(int w, int h) = 0;
  virtual void apply_config() = 0;
  virtual Vector to_logical(int physical_x, int physical_y) const = 0;
  virtual void set_gamma(float gamma) = 0;
  virtual SDL_Window* get_window() const = 0;
};

#endif // HEADER_SUPERTUX_VIDEO_RENDERER_HPP

// EOF
