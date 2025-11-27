// src/video/gl/gl_surface_data.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_GL_SURFACE_DATA_HPP
#define HEADER_SUPERTUX_VIDEO_GL_SURFACE_DATA_HPP

#include "video/surface.hpp"
#include "video/surface_data.hpp"

class GLSurfaceData : public SurfaceData
{
private:
  const Surface &surface;
  float uv_left;
  float uv_top;
  float uv_right;
  float uv_bottom;

public:
  GLSurfaceData(const Surface& surface_) :
    surface(surface_),
    uv_left((float) surface.get_x() / surface.get_texture()->get_texture_width()),
    uv_top((float) surface.get_y() / surface.get_texture()->get_texture_height()),
    uv_right((float) (surface.get_x() + surface.get_width()) / surface.get_texture()->get_texture_width()),
    uv_bottom((float) (surface.get_y() + surface.get_height()) / surface.get_texture()->get_texture_height())
  {
  }

  float get_uv_left() const
  {
    return surface.get_flipx() ? uv_right : uv_left;
  }

  float get_uv_top() const
  {
    return uv_top;
  }

  float get_uv_right() const
  {
    return surface.get_flipx() ? uv_left : uv_right;
  }

  float get_uv_bottom() const
  {
    return uv_bottom;
  }
};

#endif // HEADER_SUPERTUX_VIDEO_GL_SURFACE_DATA_HPP

// EOF
