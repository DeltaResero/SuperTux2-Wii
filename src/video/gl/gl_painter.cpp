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

#include "video/gl/gl_painter.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <utility>

#include "math/util.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_request.hpp"
#include "video/gl/gl_context.hpp"
#include "video/gl/gl_program.hpp"
#include "video/gl/gl_renderer.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/gl/gl_vertex_arrays.hpp"
#include "video/gl/gl_video_system.hpp"
#include "video/glutil.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

inline std::tuple<GLenum, GLenum> blend_factor(Blend blend)
{
  using B = std::tuple<GLenum, GLenum>;
  switch(blend) {
    case Blend::NONE: return B(GL_ONE, GL_ZERO);
    case Blend::BLEND: return B(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    case Blend::ADD: return B(GL_SRC_ALPHA, GL_ONE);
    case Blend::MOD: return B(GL_DST_COLOR, GL_ZERO);
    default: return B(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

inline GLenum sfactor(Blend blend)
{
  return std::get<0>(blend_factor(blend));
}

inline GLenum dfactor(Blend blend)
{
  return std::get<1>(blend_factor(blend));
}

} // namespace

GLPainter::GLPainter(GLVideoSystem& video_system, GLRenderer& renderer) :
  m_video_system(video_system),
  m_renderer(renderer),
  m_vertices(),
  m_uvs(),
  m_shown(),
  m_colors()
{
}

void
GLPainter::draw_texture(const TextureRequest& request)
{
  if (!request.tints.empty())
  {
    draw_tinted(request);
    return;
  }

  assert_gl();

  const auto& texture = static_cast<const GLTexture&>(*request.texture);

  assert(request.srcrects.size() == request.dstrects.size());
  assert(request.srcrects.size() == request.angles.size());

  // With nothing turned or faded, every picture is 6 corners, so the lists only grow and the corners go straight in
  const bool plain = std::all_of(request.angles.begin(), request.angles.end(), [](float angle) { return angle == 0.0f; }) &&
                     std::all_of(request.fades.begin(), request.fades.end(), [](uint8_t fades) { return fades == 0; });
  const size_t plain_floats = request.srcrects.size() * 12;
  if (plain)
  {
    if (m_vertices.size() < plain_floats) m_vertices.resize(plain_floats);
    if (m_uvs.size() < plain_floats) m_uvs.resize(plain_floats);
  }
  else
  {
    m_vertices.clear();
    m_uvs.clear();
    m_shown.clear();
    m_vertices.reserve(request.srcrects.size() * 12);
    m_uvs.reserve(request.srcrects.size() * 12);
    m_shown.reserve(request.srcrects.size() * 6);
  }
  bool faded = false;

  for (size_t i = 0; i < request.srcrects.size(); ++i)
  {
    const Rectf dstrect = to_target(request.dstrects[i], request.between_pixels);
    const float left = dstrect.get_left();
    const float top = dstrect.get_top();
    const float right  = dstrect.get_right();
    const float bottom = dstrect.get_bottom();

    const Vector& uv_scale = texture.get_uv_scale();
    float uv_left = request.srcrects[i].get_left() * uv_scale.x;
    float uv_top = request.srcrects[i].get_top() * uv_scale.y;
    float uv_right = request.srcrects[i].get_right() * uv_scale.x;
    float uv_bottom = request.srcrects[i].get_bottom() * uv_scale.y;

    if (request.flip & HORIZONTAL_FLIP)
      std::swap(uv_left, uv_right);

    if (request.flip & VERTICAL_FLIP)
      std::swap(uv_top, uv_bottom);

    const uint8_t fades = (i < request.fades.size()) ? request.fades[i] : 0;
    if (fades != 0 && request.between_pixels && request.angles[i] == 0.0f && !m_renderer.get_texture() &&
        !request.displacement_texture && texture.get_sampler().get_animate() == Vector(0.0f, 0.0f))
    {
      add_faded(request.srcrects[i], texture, fades, request.flip, left, top, right, bottom, uv_left, uv_top, uv_right, uv_bottom);
      faded = true;
    }
    else if (request.angles[i] == 0.0f)
    {
      // Written in place, as a range insert per picture costs more than the drawing in a build without optimising
      const size_t at = plain ? i * 12 : m_vertices.size();
      const size_t uv_at = plain ? i * 12 : m_uvs.size();
      if (!plain)
      {
        m_vertices.resize(at + 12);
        m_uvs.resize(uv_at + 12);
        m_shown.resize(m_shown.size() + 6, 1.0f);
      }
      float* vertex = &m_vertices[at];
      float* uv = &m_uvs[uv_at];
      vertex[0] = left;   vertex[1] = top;     uv[0] = uv_left;   uv[1] = uv_top;
      vertex[2] = right;  vertex[3] = top;     uv[2] = uv_right;  uv[3] = uv_top;
      vertex[4] = right;  vertex[5] = bottom;  uv[4] = uv_right;  uv[5] = uv_bottom;
      vertex[6] = left;   vertex[7] = bottom;  uv[6] = uv_left;   uv[7] = uv_bottom;
      vertex[8] = left;   vertex[9] = top;     uv[8] = uv_left;   uv[9] = uv_top;
      vertex[10] = right; vertex[11] = bottom; uv[10] = uv_right; uv[11] = uv_bottom;
    }
    else
    {
      // rotated blit
      const float center_x = (left + right) / 2;
      const float center_y = (top + bottom) / 2;

      const float sa = sinf(math::radians(request.angles[i]));
      const float ca = cosf(math::radians(request.angles[i]));

      const float new_left = left - center_x;
      const float new_right = right - center_x;

      const float new_top = top - center_y;
      const float new_bottom = bottom - center_y;

      const float vertices_lst[] = {
        new_left*ca - new_top*sa + center_x, new_left*sa + new_top*ca + center_y,
        new_right*ca - new_top*sa + center_x, new_right*sa + new_top*ca + center_y,
        new_right*ca - new_bottom*sa + center_x, new_right*sa + new_bottom*ca + center_y,

        new_left*ca - new_bottom*sa + center_x, new_left*sa + new_bottom*ca + center_y,
        new_left*ca - new_top*sa + center_x, new_left*sa + new_top*ca + center_y,
        new_right*ca - new_bottom*sa + center_x, new_right*sa + new_bottom*ca + center_y,
      };
      m_vertices.insert(m_vertices.end(), std::begin(vertices_lst), std::end(vertices_lst));

      const float uvs_lst[] = {
        uv_left, uv_top,
        uv_right, uv_top,
        uv_right, uv_bottom,

        uv_left, uv_bottom,
        uv_left, uv_top,
        uv_right, uv_bottom,
      };
      m_uvs.insert(m_uvs.end(), std::begin(uvs_lst), std::end(uvs_lst));
      m_shown.insert(m_shown.end(), 6, 1.0f);
    }
  }

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_texture(texture, request.displacement_texture);
  const size_t floats = plain ? plain_floats : m_vertices.size();
  context.set_texcoords(m_uvs.data(), sizeof(float) * (plain ? plain_floats : m_uvs.size()));
  context.set_positions(m_vertices.data(), sizeof(float) * floats);
  const Color color(request.color.red, request.color.green, request.color.blue, request.color.alpha * request.alpha);
  std::vector<float> colors;
  if (faded)
  {
    // A fading edge needs its own alpha at each corner
    colors.reserve(m_shown.size() * 4);
    for (float shown : m_shown)
    {
      colors.insert(colors.end(), { color.red, color.green, color.blue, color.alpha * shown });
    }
    context.set_colors(colors.data(), sizeof(float) * colors.size());
  }
  else
  {
    context.set_color(color);
  }

  context.draw_arrays(GL_TRIANGLES, 0, static_cast<GLsizei>(floats / 2));

  assert_gl();
}

void
GLPainter::draw_tinted(const TextureRequest& request)
{
  assert_gl();

  const auto& texture = static_cast<const GLTexture&>(*request.texture);
  const Vector uv_scale = texture.get_uv_scale();
  // Written in place like draw_texture's, and the lists only grow
  const size_t count = request.dstrects.size();
  if (m_vertices.size() < count * 12) m_vertices.resize(count * 12);
  if (m_uvs.size() < count * 12) m_uvs.resize(count * 12);
  if (m_colors.size() < count * 24) m_colors.resize(count * 24);
  for (size_t i = 0; i < count; ++i)
  {
    const Rectf dstrect = to_target(request.dstrects[i], request.between_pixels);
    const Rectf& src = request.srcrects[i];
    float uv_left = src.get_left() * uv_scale.x;
    float uv_right = src.get_right() * uv_scale.x;
    float uv_top = src.get_top() * uv_scale.y;
    float uv_bottom = src.get_bottom() * uv_scale.y;
    if (request.flip & HORIZONTAL_FLIP) std::swap(uv_left, uv_right);
    if (request.flip & VERTICAL_FLIP) std::swap(uv_top, uv_bottom);

    // Corners from the top left round, as two triangles
    const float xs[4] = { dstrect.get_left(), dstrect.get_right(), dstrect.get_right(), dstrect.get_left() };
    const float ys[4] = { dstrect.get_top(), dstrect.get_top(), dstrect.get_bottom(), dstrect.get_bottom() };
    const float us[4] = { uv_left, uv_right, uv_right, uv_left };
    const float vs[4] = { uv_top, uv_top, uv_bottom, uv_bottom };
    const size_t corners[6] = { 0, 1, 2, 0, 2, 3 };
    float* vertex = &m_vertices[i * 12];
    float* uv = &m_uvs[i * 12];
    float* color = &m_colors[i * 24];
    for (size_t k = 0; k < 6; ++k)
    {
      const size_t corner = corners[k];
      const Color& tint = request.tints[i][corner];
      vertex[k * 2] = xs[corner];
      vertex[k * 2 + 1] = ys[corner];
      uv[k * 2] = us[corner];
      uv[k * 2 + 1] = vs[corner];
      color[k * 4] = request.color.red * tint.red;
      color[k * 4 + 1] = request.color.green * tint.green;
      color[k * 4 + 2] = request.color.blue * tint.blue;
      color[k * 4 + 3] = request.color.alpha * request.alpha * tint.alpha;
    }
  }

  GLContext& context = m_video_system.get_context();
  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_texture(texture, nullptr);
  context.set_texcoords(m_uvs.data(), sizeof(float) * count * 12);
  context.set_positions(m_vertices.data(), sizeof(float) * count * 12);
  context.set_colors(m_colors.data(), sizeof(float) * count * 24);
  context.draw_arrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count * 6));

  assert_gl();
}

void
GLPainter::add_faded(const Rectf& src, const GLTexture& texture, uint8_t fades, Flip flip,
                     float left, float top, float right, float bottom,
                     float uv_left, float uv_top, float uv_right, float uv_bottom)
{
  const Rect viewport = m_renderer.get_rect();
  const Sizef logical_size = m_renderer.get_logical_size();
  const Vector half(0.5f * logical_size.width / static_cast<float>(viewport.get_width()),
                    0.5f * logical_size.height / static_cast<float>(viewport.get_height()));

  // A side cut from inside a bigger texture doesn't fade, or it would show what lies beside it
  bool cut_left = src.get_left() > 0.0f, cut_right = src.get_right() < static_cast<float>(texture.get_image_width());
  bool cut_top = src.get_top() > 0.0f, cut_bottom = src.get_bottom() < static_cast<float>(texture.get_image_height());
  if (flip & HORIZONTAL_FLIP) std::swap(cut_left, cut_right);
  if (flip & VERTICAL_FLIP) std::swap(cut_top, cut_bottom);

  const bool wide = right - left > 4.0f * half.x, tall = bottom - top > 4.0f * half.y;
  const auto xs = fade_lines(left, right, half.x, wide && (fades & FADE_LEFT) && !cut_left, wide && (fades & FADE_RIGHT) && !cut_right);
  const auto ys = fade_lines(top, bottom, half.y, tall && (fades & FADE_TOP) && !cut_top, tall && (fades & FADE_BOTTOM) && !cut_bottom);

  const auto corner = [&](size_t column, size_t row) {
    const float x = xs[column].first, y = ys[row].first;
    m_vertices.insert(m_vertices.end(), { x, y });
    m_uvs.insert(m_uvs.end(), { uv_left + std::clamp((x - left) / (right - left), 0.0f, 1.0f) * (uv_right - uv_left),
                                uv_top + std::clamp((y - top) / (bottom - top), 0.0f, 1.0f) * (uv_bottom - uv_top) });
    m_shown.push_back(xs[column].second * ys[row].second);
  };
  for (size_t row = 0; row + 1 < ys.size(); ++row)
  {
    for (size_t column = 0; column + 1 < xs.size(); ++column)
    {
      corner(column, row); corner(column + 1, row); corner(column + 1, row + 1);
      corner(column, row); corner(column + 1, row + 1); corner(column, row + 1);
    }
  }
}

Rectf
GLPainter::to_target(const Rectf& rect, bool between_pixels) const
{
  // Fractions kept for a gliding layer, and on the lightmap, where whole pixels would move a light in steps
  if (m_renderer.get_texture() || between_pixels)
  {
    return rect;
  }

  // The viewport starts on a whole pixel, so both edges go to the nearest whole screen pixel from its corner
  const Rect viewport = m_renderer.get_rect();
  const Sizef logical_size = m_renderer.get_logical_size();
  const float scale_x = static_cast<float>(viewport.get_width()) / logical_size.width;
  const float scale_y = static_cast<float>(viewport.get_height()) / logical_size.height;
  return Rectf(std::round(rect.get_left() * scale_x) / scale_x,
               std::round(rect.get_top() * scale_y) / scale_y,
               std::round(rect.get_right() * scale_x) / scale_x,
               std::round(rect.get_bottom() * scale_y) / scale_y);
}

void
GLPainter::draw_gradient(const GradientRequest& request)
{
  assert_gl();

  const Color& top = request.top;
  const Color& bottom = request.bottom;
  const GradientDirection& direction = request.direction;
  const Rectf& region = request.region;

  GLContext& context = m_video_system.get_context();

  const float vertices[] = {
    region.get_left(), region.get_top(),
    region.get_right(), region.get_top(),
    region.get_right(), region.get_bottom(),
    region.get_left(), region.get_bottom()
  };

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_positions(vertices, sizeof(vertices));
  context.set_texcoord(0.0f, 0.0f);

  if (direction == VERTICAL || direction == VERTICAL_SECTOR)
  {
    const float colors[] = {
      top.red, top.green, top.blue, top.alpha,
      top.red, top.green, top.blue, top.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
    };
    context.set_colors(colors, sizeof(colors));
  }
  else
  {
    const float colors[] = {
      top.red, top.green, top.blue, top.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
      bottom.red, bottom.green, bottom.blue, bottom.alpha,
      top.red, top.green, top.blue, top.alpha,
    };
    context.set_colors(colors, sizeof(colors));
  }

  context.draw_arrays(GL_TRIANGLE_FAN, 0, 4);

  assert_gl();
}

void
GLPainter::draw_filled_rect(const FillRectRequest& request)
{
  assert_gl();

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_texcoord(0.0f, 0.0f);
  context.set_color(request.color);

  if (request.radius != 0.0f)
  {
    // draw round rect
    // Keep radius in the limits, so that we get a circle instead of
    // just graphic junk
    const float radius = std::min(request.radius,
                                  std::min(request.rect.get_width() / 2.0f,
                                           request.rect.get_height() / 2.0f));

    // inner rectangle, which a box no bigger than its corners could turn inside out by a float error
    const Rectf irect(request.rect.get_left() + radius,
                      request.rect.get_top() + radius,
                      std::max(request.rect.get_left() + radius, request.rect.get_right() - radius),
                      std::max(request.rect.get_top() + radius, request.rect.get_bottom() - radius));

    const int n = 8;
    size_t p = 0;
    std::vector<float> vertices((n+1) * 4 * 2);

    for (int i = 0; i <= n; ++i)
    {
      const float x = sinf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;
      const float y = cosf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;

      vertices[p++] = irect.get_left() - x;
      vertices[p++] = irect.get_top()  - y;

      vertices[p++] = irect.get_right() + x;
      vertices[p++] = irect.get_top()   - y;
    }

    for (int i = 0; i <= n; ++i)
    {
      const float x = cosf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;
      const float y = sinf(static_cast<float>(i) * math::PI_2 / static_cast<float>(n)) * radius;

      vertices[p++] = irect.get_left()   - x;
      vertices[p++] = irect.get_bottom() + y;

      vertices[p++] = irect.get_right()  + x;
      vertices[p++] = irect.get_bottom() + y;
    }

    context.set_positions(vertices.data(), sizeof(float) * vertices.size());

    context.draw_arrays(GL_TRIANGLE_STRIP, 0,  static_cast<GLsizei>(vertices.size() / 2));
  }
  else
  {
    const float x = request.rect.get_left();
    const float y = request.rect.get_top();
    const float w = request.rect.get_width();
    const float h = request.rect.get_height();

    const float vertices[] = {
      x,   y,
      x+w, y,
      x+w, y+h,
      x,   y+h
    };

    context.set_positions(vertices, sizeof(vertices));

    context.draw_arrays(GL_TRIANGLE_FAN, 0, 4);
  }

  assert_gl();
}

void
GLPainter::draw_inverse_ellipse(const InverseEllipseRequest& request)
{
  assert_gl();

  const float x = request.pos.x;
  const float y = request.pos.y;
  const float w = request.size.x/2.0f;
  const float h = request.size.y/2.0f;

  constexpr int slices = 16;
  constexpr int points = (slices+1) * 12;

  float vertices[points * 2];
  int p = 0;

  const Viewport& viewport = m_video_system.get_viewport();
  const float screen_width = static_cast<float>(viewport.get_screen_width());
  const float screen_height = static_cast<float>(viewport.get_screen_height());

  // Bottom
  vertices[p++] = screen_width; vertices[p++] = screen_height;
  vertices[p++] = 0;            vertices[p++] = screen_height;
  vertices[p++] = x;            vertices[p++] = y+h;

  // Top
  vertices[p++] = screen_width; vertices[p++] = 0;
  vertices[p++] = 0;            vertices[p++] = 0;
  vertices[p++] = x;            vertices[p++] = y-h;

  // Left
  vertices[p++] = screen_width; vertices[p++] = 0;
  vertices[p++] = screen_width; vertices[p++] = screen_height;
  vertices[p++] = x+w;          vertices[p++] = y;

  // Right
  vertices[p++] = 0;            vertices[p++] = 0;
  vertices[p++] = 0;            vertices[p++] = screen_height;
  vertices[p++] = x-w;          vertices[p++] = y;

  for (int i = 0; i < slices; ++i)
  {
    const float ex1 = sinf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i)) * w;
    const float ey1 = cosf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i)) * h;

    const float ex2 = sinf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i+1)) * w;
    const float ey2 = cosf(math::PI_2 / static_cast<float>(slices) * static_cast<float>(i+1)) * h;

    // Bottom/Right
    vertices[p++] = screen_width; vertices[p++] = screen_height;
    vertices[p++] = x + ex1;      vertices[p++] = y + ey1;
    vertices[p++] = x + ex2;      vertices[p++] = y + ey2;

    // Top/Left
    vertices[p++] = 0;            vertices[p++] = 0;
    vertices[p++] = x - ex1;      vertices[p++] = y - ey1;
    vertices[p++] = x - ex2;      vertices[p++] = y - ey2;

    // Top/Right
    vertices[p++] = screen_width; vertices[p++] = 0;
    vertices[p++] = x + ex1;      vertices[p++] = y - ey1;
    vertices[p++] = x + ex2;      vertices[p++] = y - ey2;

    // Bottom/Left
    vertices[p++] = 0;            vertices[p++] = screen_height;
    vertices[p++] = x - ex1;      vertices[p++] = y + ey1;
    vertices[p++] = x - ex2;      vertices[p++] = y + ey2;
  }

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_positions(vertices, sizeof(vertices));
  context.set_texcoord(0.0f, 0.0f);
  context.set_color(request.color);

  context.draw_arrays(GL_TRIANGLES, 0, points);

  assert_gl();
}

void
GLPainter::draw_line(const LineRequest& request)
{
  assert_gl();

  Vector viewport_scale = m_video_system.get_viewport().get_scale();
  const float x1 = request.pos.x;
  const float y1 = request.pos.y;
  const float x2 = request.dest_pos.x;
  const float y2 = request.dest_pos.y;

  // OpenGL3.3 doesn't have GL_LINES anymore, so instead we transform
  // the line into a quad and draw it as triangle strip.
  float x_step = (y2 - y1);
  float y_step = -(x2 - x1);

  const float step_norm = sqrtf(x_step * x_step + y_step * y_step);
  x_step /= step_norm * viewport_scale.x;
  y_step /= step_norm * viewport_scale.y;

  x_step *= 0.5f;
  y_step *= 0.5f;

  const float vertices[] = {
    (x1 - x_step), (y1 - y_step),
    (x2 - x_step), (y2 - y_step),
    (x1 + x_step), (y1 + y_step),
    (x2 + x_step), (y2 + y_step),
  };

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_positions(vertices, sizeof(vertices));
  context.set_texcoord(0.0f, 0.0f);
  context.set_color(request.color);

  context.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

  assert_gl();
}

void
GLPainter::draw_triangle(const TriangleRequest& request)
{
  assert_gl();

  const float vertices[] = {
    request.pos1.x, request.pos1.y,
    request.pos2.x, request.pos2.y,
    request.pos3.x, request.pos3.y,
  };

  GLContext& context = m_video_system.get_context();

  context.blend_func(sfactor(request.blend), dfactor(request.blend));
  context.bind_no_texture();
  context.set_texcoord(0.0f, 0.0f);
  context.set_positions(vertices, sizeof(vertices));
  context.set_color(request.color);

  context.draw_arrays(GL_TRIANGLES, 0, 3);

  assert_gl();
}

void
GLPainter::clear(const Color& color)
{
  assert_gl();

  glClearColor(color.red, color.green, color.blue, color.alpha);
  glClear(GL_COLOR_BUFFER_BIT);

  assert_gl();
}

void
GLPainter::get_pixel(const GetPixelRequest& request) const
{
  assert_gl();

  const Rect& rect = m_renderer.get_rect();
  const Sizef logical_size = m_renderer.get_logical_size();

  float x = request.pos.x * static_cast<float>(rect.get_width()) / logical_size.width;
  float y = request.pos.y * static_cast<float>(rect.get_height()) / logical_size.height;

  x += static_cast<float>(rect.left);
  y += static_cast<float>(rect.top);

  float pixels[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

  // OpenGLES2 does not have PBOs, only GLES3 has
  glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y),
               1, 1, GL_RGB, GL_FLOAT, pixels);

  *(request.color_ptr) = Color(pixels[0], pixels[1], pixels[2]);

  assert_gl();
}

void
GLPainter::set_clip_rect(const Rect& clip_rect)
{
  assert_gl();

  const Rect& rect = m_renderer.get_rect();
  const Sizef logical_size = m_renderer.get_logical_size();
  const float scale_x = static_cast<float>(rect.get_width()) / logical_size.width;
  const float scale_y = static_cast<float>(rect.get_height()) / logical_size.height;

  glScissor(rect.left + static_cast<int>(static_cast<float>(clip_rect.left) * scale_x),
            rect.top + static_cast<int>(static_cast<float>(clip_rect.top) * scale_y),
            static_cast<int>(static_cast<float>(clip_rect.get_width()) * scale_x),
            static_cast<int>(static_cast<float>(clip_rect.get_height()) * scale_y));
  glEnable(GL_SCISSOR_TEST);

  assert_gl();
}

void
GLPainter::clear_clip_rect()
{
  assert_gl();

  glDisable(GL_SCISSOR_TEST);

  assert_gl();
}

/* EOF */
