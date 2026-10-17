//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_DRAWING_TRANSFORM_HPP
#define HEADER_SUPERTUX_VIDEO_DRAWING_TRANSFORM_HPP

#include "math/vector.hpp"
#include "video/texture.hpp"

class DrawingTransform final
{
public:
  Vector translation;
  Flip flip;
  float alpha;
  float scale;

  /** Rounds each picture and the camera to whole pixels apart, so things standing still keep their place against the ground */
  bool pixel_grid;

  /** Pictures keep their fractions, so things glide at any speed instead of stepping a whole pixel now and then */
  bool between_pixels;

  /** Between pixels, a picture's edges fade out over a pixel so they glide like its insides; off for pictures that sit edge to edge */
  bool fade_edges;

  DrawingTransform() :
    translation(0.0f, 0.0f),
    flip(NO_FLIP),
    alpha(1.0f),
    scale(1.0f),
    pixel_grid(false),
    between_pixels(false),
    fade_edges(true)
  {}
};

#endif

/* EOF */
