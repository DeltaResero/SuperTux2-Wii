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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_PAINTER_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_PAINTER_HPP

#include "video/painter.hpp"

#include <cstdint>
#include <vector>

#include "video/flip.hpp"

enum class Blend;
class GLRenderer;
class GLTexture;
class GLVideoSystem;
class Rectf;

class GLPainter final : public Painter
{
public:
  GLPainter(GLVideoSystem& video_system, GLRenderer& renderer);

  virtual void draw_texture(const TextureRequest& request) override;
  virtual void draw_gradient(const GradientRequest& request) override;
  virtual void draw_filled_rect(const FillRectRequest& request) override;
  virtual void draw_inverse_ellipse(const InverseEllipseRequest& request) override;
  virtual void draw_line(const LineRequest& request) override;
  virtual void draw_triangle(const TriangleRequest& request) override;

  virtual void clear(const Color& color) override;
  virtual void get_pixel(const GetPixelRequest& request) const override;

  virtual void set_clip_rect(const Rect& rect) override;
  virtual void clear_clip_rect() override;

private:
  Rectf to_target(const Rectf& rect, bool between_pixels) const;

  /** Adds the picture with its FADE_ sides fading out over 1 screen pixel, so a pixel on the edge shows as much as it covers */
  void add_faded(const Rectf& src, const GLTexture& texture, uint8_t fades, Flip flip,
                 float left, float top, float right, float bottom,
                 float uv_left, float uv_top, float uv_right, float uv_bottom);

private:
  GLVideoSystem& m_video_system;
  GLRenderer& m_renderer;

private:
  std::vector<float> m_vertices;
  std::vector<float> m_uvs;
  std::vector<float> m_shown;

private:
  GLPainter(const GLPainter&) = delete;
  GLPainter& operator=(const GLPainter&) = delete;
};

#endif

/* EOF */
