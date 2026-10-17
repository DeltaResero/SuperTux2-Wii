//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_VIDEO_DRAWING_REQUEST_HPP
#define HEADER_SUPERTUX_VIDEO_DRAWING_REQUEST_HPP

#include <cstdint>
#include <string>
#include <memory>

#include "math/rectf.hpp"
#include "math/sizef.hpp"
#include "math/vector.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/font.hpp"

class Surface;

/** Sides of a picture that fade out over a pixel when it lands between pixels */
enum : uint8_t
{
  FADE_LEFT = 1,
  FADE_RIGHT = 2,
  FADE_TOP = 4,
  FADE_BOTTOM = 8,
  FADE_ALL = FADE_LEFT | FADE_RIGHT | FADE_TOP | FADE_BOTTOM
};

enum RequestType
{
  TEXTURE, GRADIENT, FILLRECT, INVERSEELLIPSE, GETPIXEL, LINE, TRIANGLE
};

struct DrawingRequest
{
  RequestType type;

  int layer;
  Flip flip;
  float alpha;
  Blend blend;

  DrawingRequest() = delete;
  DrawingRequest(RequestType type_) :
    type(type_),
    layer(),
    flip(),
    alpha(),
    blend()
  {}
  virtual ~DrawingRequest() {}
};

struct TextureRequest : public DrawingRequest
{
  TextureRequest() :
    DrawingRequest(TEXTURE),
    texture(),
    displacement_texture(),
    srcrects(),
    dstrects(),
    angles(),
    color(1.0f, 1.0f, 1.0f),
    between_pixels(false),
    fades()
  {}

  const Texture* texture;
  const Texture* displacement_texture;
  std::vector<Rectf> srcrects;
  std::vector<Rectf> dstrects;
  std::vector<float> angles;
  Color color;
  bool between_pixels;
  std::vector<uint8_t> fades;

private:
  TextureRequest(const TextureRequest&) = delete;
  TextureRequest& operator=(const TextureRequest&) = delete;
};

struct GradientRequest : public DrawingRequest
{
  GradientRequest()  :
    DrawingRequest(GRADIENT),
    pos(0.0f, 0.0f),
    size(0.0f, 0.0f),
    top(),
    bottom(),
    direction(),
    region()
  {}

  Vector pos;
  Vector size;
  Color top;
  Color bottom;
  GradientDirection direction;
  Rectf region;
};

struct FillRectRequest : public DrawingRequest
{
  FillRectRequest() :
    DrawingRequest(FILLRECT),
    rect(),
    color(),
    radius(),
    whole_pixels(false)
  {}

  Rectf rect;
  Color color;
  float radius;
  /** Edges on whole screen pixels, as pictures on the pixel grid have */
  bool whole_pixels;
};

struct InverseEllipseRequest : public DrawingRequest
{
  InverseEllipseRequest() :
    DrawingRequest(INVERSEELLIPSE),
    pos(0.0f, 0.0f),
    size(0.0f, 0.0f),
    color()
  {}

  Vector pos;
  Vector size;
  Color color;
};

struct LineRequest : public DrawingRequest
{
  LineRequest() :
    DrawingRequest(LINE),
    pos(0.0f, 0.0f),
    dest_pos(0.0f, 0.0f),
    color()
  {}

  Vector pos;
  Vector dest_pos;
  Color color;
};

struct TriangleRequest : public DrawingRequest
{
  TriangleRequest() :
    DrawingRequest(TRIANGLE),
    pos1(0.0f, 0.0f),
    pos2(0.0f, 0.0f),
    pos3(0.0f, 0.0f),
    color()
  {}

  Vector pos1, pos2, pos3;
  Color  color;
};

struct GetPixelRequest : public DrawingRequest
{
  GetPixelRequest() :
    DrawingRequest(GETPIXEL),
    pos(0.0f, 0.0f),
    color_ptr() {}

  Vector pos;
  std::shared_ptr<Color> color_ptr;

private:
  GetPixelRequest(const GetPixelRequest&) = delete;
  GetPixelRequest& operator=(const GetPixelRequest&) = delete;
};

#endif

/* EOF */
