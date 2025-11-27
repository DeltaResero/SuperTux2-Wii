// src/video/lightmap.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_LIGHTMAP_HPP
#define HEADER_SUPERTUX_VIDEO_LIGHTMAP_HPP

#include <SDL_video.h>
#include <memory>
#include <obstack.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "math/rectf.hpp"
#include "math/vector.hpp"
#include "video/color.hpp"
#include "video/drawing_request.hpp"
#include "video/font.hpp"
#include "video/glutil.hpp"
#include "video/surface.hpp"

class Texture;
struct DrawingRequest;

class Lightmap
{
public:
  virtual ~Lightmap() {}

  virtual void start_draw(const Color &ambient_color) = 0;
  virtual void end_draw() = 0;
  virtual void do_draw() = 0;
  virtual void draw_surface(const DrawingRequest& request) = 0;
  virtual void draw_surface_part(const DrawingRequest& request) = 0;
  virtual void draw_gradient(const DrawingRequest& request) = 0;
  virtual void draw_filled_rect(const DrawingRequest& request) = 0;
  virtual void draw_inverse_ellipse(const DrawingRequest& request) = 0;
  virtual void get_light(const DrawingRequest& request) const = 0;
  virtual void draw_line(const DrawingRequest& request) = 0;
  virtual void draw_triangle(const DrawingRequest& request) = 0;
};

#endif // HEADER_SUPERTUX_VIDEO_LIGHTMAP_HPP

// EOF
