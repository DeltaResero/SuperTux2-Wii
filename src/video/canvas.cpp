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

#include "video/canvas.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "util/arena.hpp"
#include "video/drawing_request.hpp"
#include "video/painter.hpp"
#include "video/renderer.hpp"
#include "video/surface.hpp"
#include "video/texture.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

bool passes(Canvas::Filter filter, const DrawingRequest& request)
{
  return !(filter == Canvas::BELOW_LIGHTMAP && request.layer >= LAYER_LIGHTMAP) &&
         !(filter == Canvas::ABOVE_LIGHTMAP && request.layer <= LAYER_LIGHTMAP);
}

// Part of a shiny picture that shows what's as far above its axis as it lies below, and how much shows at each corner
struct Pane
{
  Rectf area;
  std::array<float, 4> shown;
};

// Where a shiny picture shows a reflection, and the request it reflects from as nothing under that shows
struct Mirror
{
  float axis;
  Rectf area;
  std::vector<Pane> panes;
  size_t start;
};

// How much shows at x, y of a pane, blending between its corners
float shown_at(const Pane& pane, float x, float y)
{
  const float across = (x - pane.area.get_left()) / pane.area.get_width();
  const float down = (y - pane.area.get_top()) / pane.area.get_height();
  const float top = pane.shown[0] + (pane.shown[1] - pane.shown[0]) * across;
  const float bottom = pane.shown[3] + (pane.shown[2] - pane.shown[3]) * across;
  return top + (bottom - top) * down;
}

// Adds the parts of area that request draws solid, so nothing drawn before it shows there
void add_solid(const DrawingRequest& request, const Rectf& area, std::vector<Rectf>& solid)
{
  if (request.blend != Blend::BLEND && request.blend != Blend::NONE)
  {
    return;
  }
  const auto add = [&](const Rectf& rect) {
    const float left = std::max(rect.get_left(), area.get_left());
    const float top = std::max(rect.get_top(), area.get_top());
    const float right = std::min(rect.get_right(), area.get_right());
    const float bottom = std::min(rect.get_bottom(), area.get_bottom());
    if (left < right && top < bottom) solid.emplace_back(left, top, right, bottom);
  };
  if (request.type == TEXTURE)
  {
    const auto& picture = static_cast<const TextureRequest&>(request);
    if (!picture.texture || !picture.texture->is_opaque() || picture.displacement_texture || !picture.shines.empty() ||
        picture.alpha * picture.color.alpha < 1.0f)
    {
      return;
    }
    for (size_t i = 0; i < picture.dstrects.size(); ++i)
    {
      if (picture.angles[i] == 0.0f) add(picture.dstrects[i]);
    }
  }
  else if (request.type == GRADIENT)
  {
    const auto& gradient = static_cast<const GradientRequest&>(request);
    if (gradient.top.alpha >= 1.0f && gradient.bottom.alpha >= 1.0f) add(gradient.region);
  }
}

// Whether solid covers all of area: cut at every edge, each piece lies inside one of them, slivers from rounding aside
bool covers(const std::vector<Rectf>& solid, const Rectf& area)
{
  std::vector<float> xs{ area.get_left(), area.get_right() };
  std::vector<float> ys{ area.get_top(), area.get_bottom() };
  for (const Rectf& rect : solid)
  {
    xs.insert(xs.end(), { rect.get_left(), rect.get_right() });
    ys.insert(ys.end(), { rect.get_top(), rect.get_bottom() });
  }
  std::sort(xs.begin(), xs.end());
  std::sort(ys.begin(), ys.end());
  for (size_t j = 0; j + 1 < ys.size(); ++j)
  {
    for (size_t i = 0; i + 1 < xs.size(); ++i)
    {
      if (xs[i + 1] - xs[i] < 0.01f || ys[j + 1] - ys[j] < 0.01f) continue;
      const Vector middle((xs[i] + xs[i + 1]) / 2.0f, (ys[j] + ys[j + 1]) / 2.0f);
      if (std::none_of(solid.begin(), solid.end(), [&middle](const Rectf& rect) { return rect.contains(middle); }))
      {
        return false;
      }
    }
  }
  return true;
}

// The part of texels that shows from left to right and top to bottom of dst, as flip lays it out
Rectf part_of(const Rectf& texels, const Rectf& dst, Flip flip, float left, float top, float right, float bottom)
{
  const float scale_x = texels.get_width() / dst.get_width();
  const float scale_y = texels.get_height() / dst.get_height();
  float part_left = (left - dst.get_left()) * scale_x;
  float part_right = (right - dst.get_left()) * scale_x;
  float part_top = (top - dst.get_top()) * scale_y;
  float part_bottom = (bottom - dst.get_top()) * scale_y;
  if (flip & HORIZONTAL_FLIP)
  {
    std::tie(part_left, part_right) = std::make_pair(texels.get_width() - part_right, texels.get_width() - part_left);
  }
  if (flip & VERTICAL_FLIP)
  {
    std::tie(part_top, part_bottom) = std::make_pair(texels.get_height() - part_bottom, texels.get_height() - part_top);
  }
  return Rectf(texels.get_left() + part_left, texels.get_top() + part_top,
               texels.get_left() + part_right, texels.get_top() + part_bottom);
}

} // namespace

Canvas::Canvas(DrawingContext& context, Arena& arena) :
  m_context(context),
  m_arena(arena),
  m_requests()
{
  m_requests.reserve(500);
}

Canvas::~Canvas()
{
  clear();
}

void
Canvas::clear()
{
  for (auto& request : m_requests)
  {
    request->~DrawingRequest();
  }
  m_requests.clear();
}

bool
Canvas::bends() const
{
  return std::any_of(m_requests.begin(), m_requests.end(), [](const DrawingRequest* request) {
    return request->type == TEXTURE && static_cast<const TextureRequest*>(request)->displacement_texture;
  });
}

void
Canvas::render(Renderer& renderer, Filter filter)
{
  // On a regular level, each frame has around 50-250 requests (before
  // batching it was 1000-3000), the sort comparator function is
  // called approximatly 3-7 times for each request.
  std::stable_sort(m_requests.begin(), m_requests.end(),
                   [](const DrawingRequest* r1, const DrawingRequest* r2){
                     return r1->layer < r2->layer;
                   });

  Painter& painter = renderer.get_painter();

  for (const auto& i : m_requests) {
    const DrawingRequest& request = *i;

    if (!passes(filter, request))
      continue;

    switch (request.type) {
      case TEXTURE:
      {
        const auto& texture_request = static_cast<const TextureRequest&>(request);
        if (!texture_request.shines.empty())
        {
          draw_reflection(painter, texture_request, filter);
        }
        painter.draw_texture(texture_request);
        break;
      }

      case GRADIENT:
        painter.draw_gradient(static_cast<const GradientRequest&>(request));
        break;

      case FILLRECT:
        painter.draw_filled_rect(static_cast<const FillRectRequest&>(request));
        break;

      case INVERSEELLIPSE:
        painter.draw_inverse_ellipse(static_cast<const InverseEllipseRequest&>(request));
        break;

      case LINE:
        painter.draw_line(static_cast<const LineRequest&>(request));
        break;

      case TRIANGLE:
        painter.draw_triangle(static_cast<const TriangleRequest&>(request));
        break;

      case GETPIXEL:
        painter.get_pixel(static_cast<const GetPixelRequest&>(request));
        break;
    }
  }
}

void
Canvas::draw_reflection(Painter& painter, const TextureRequest& mirror, Filter filter) const
{
  // Upside down, its shine would look down
  if (mirror.flip & VERTICAL_FLIP)
  {
    return;
  }

  std::vector<Mirror> mirrors;
  for (const Shine& shine : mirror.shines)
  {
    const Rectf& src = shine.srcrect;
    const Rectf& dst = shine.dstrect;
    if (!shine.reflection || shine.reflection->shown.empty() || src.get_width() <= 0.0f || src.get_height() <= 0.0f)
    {
      continue;
    }
    const Reflection& reflection = *shine.reflection;
    const float scale_x = dst.get_width() / src.get_width();
    const float scale_y = dst.get_height() / src.get_height();
    const bool turned = (mirror.flip & HORIZONTAL_FLIP) != 0;
    Mirror panes{ dst.get_top() - src.get_top() * scale_y, Rectf(), {}, 0 };
    for (int row = 0; row + 1 < reflection.rows(); ++row)
    {
      const float top = std::max(reflection.y(row), src.get_top());
      const float bottom = std::min(reflection.y(row + 1), src.get_bottom());
      for (int column = 0; column + 1 < reflection.columns() && top < bottom; ++column)
      {
        const float left = std::max(reflection.x(column), src.get_left());
        const float right = std::min(reflection.x(column + 1), src.get_right());
        if (left >= right)
        {
          continue;
        }
        // A cell cut by the picture's edge takes its corners where the cut falls
        const float cell_left = reflection.x(column);
        const float cell_top = reflection.y(row);
        const float cell_width = reflection.x(column + 1) - cell_left;
        const float cell_height = reflection.y(row + 1) - cell_top;
        const auto at = [&](float x, float y) {
          const float across = (x - cell_left) / cell_width;
          const float down = (y - cell_top) / cell_height;
          const float upper = reflection.at(column, row) + (reflection.at(column + 1, row) - reflection.at(column, row)) * across;
          const float lower = reflection.at(column, row + 1) +
                              (reflection.at(column + 1, row + 1) - reflection.at(column, row + 1)) * across;
          return upper + (lower - upper) * down;
        };
        const float near = dst.get_left() + (left - src.get_left()) * scale_x;
        const float far = dst.get_left() + (right - src.get_left()) * scale_x;
        Pane pane{ Rectf(turned ? dst.get_left() + dst.get_right() - far : near, panes.axis + top * scale_y,
                         turned ? dst.get_left() + dst.get_right() - near : far, panes.axis + bottom * scale_y),
                   { at(left, top), at(right, top), at(right, bottom), at(left, bottom) } };
        if (turned)
        {
          std::swap(pane.shown[0], pane.shown[1]);
          std::swap(pane.shown[2], pane.shown[3]);
        }
        panes.area = panes.panes.empty() ? pane.area :
          Rectf(std::min(panes.area.get_left(), pane.area.get_left()), std::min(panes.area.get_top(), pane.area.get_top()),
                std::max(panes.area.get_right(), pane.area.get_right()), std::max(panes.area.get_bottom(), pane.area.get_bottom()));
        panes.panes.push_back(pane);
      }
    }
    if (!panes.panes.empty())
    {
      mirrors.push_back(std::move(panes));
    }
  }
  if (mirrors.empty())
  {
    return;
  }

  // Back from the last request, the one that leaves all of what a pane reflects covered hides everything before it
  std::vector<Rectf> solid;
  // All that any pane reflects, so a picture far from every one of them is passed over at once
  Rectf reach;
  size_t first = m_requests.size();
  for (Mirror& panes : mirrors)
  {
    const float twice = 2.0f * panes.axis;
    const Rectf above(panes.area.get_left(), twice - panes.area.get_bottom(), panes.area.get_right(), twice - panes.area.get_top());
    reach = (&panes == &mirrors.front()) ? above :
      Rectf(std::min(reach.get_left(), above.get_left()), std::min(reach.get_top(), above.get_top()),
            std::max(reach.get_right(), above.get_right()), std::max(reach.get_bottom(), above.get_bottom()));
    solid.clear();
    for (size_t i = m_requests.size(); i-- > 0;)
    {
      const size_t before = solid.size();
      if (passes(filter, *m_requests[i])) add_solid(*m_requests[i], above, solid);
      if (solid.size() != before && covers(solid, above))
      {
        panes.start = i;
        break;
      }
    }
    first = std::min(first, panes.start);
  }
  const auto near = [&reach](const Rectf& rect) {
    return rect.get_left() < reach.get_right() && rect.get_right() > reach.get_left() &&
           rect.get_top() < reach.get_bottom() && rect.get_bottom() > reach.get_top();
  };

  // Each piece of from that lies as far above an axis as a pane lies below it, upside down in that pane
  TextureRequest pieces;
  const auto reflect = [&mirrors, &pieces](size_t index, const Rectf& from, const auto& add) {
    for (const Mirror& panes : mirrors)
    {
      const float twice = 2.0f * panes.axis;
      if (index < panes.start || from.get_left() >= panes.area.get_right() || from.get_right() <= panes.area.get_left() ||
          from.get_bottom() <= twice - panes.area.get_bottom() || from.get_top() >= twice - panes.area.get_top())
      {
        continue;
      }
      for (const Pane& pane : panes.panes)
      {
        const float left = std::max(from.get_left(), pane.area.get_left());
        const float right = std::min(from.get_right(), pane.area.get_right());
        const float above = std::max(from.get_top(), twice - pane.area.get_bottom());
        const float below = std::min(from.get_bottom(), twice - pane.area.get_top());
        if (left >= right || above >= below)
        {
          continue;
        }
        const float top = twice - below;
        const float bottom = twice - above;
        pieces.dstrects.emplace_back(left, top, right, bottom);
        pieces.angles.push_back(0.0f);
        add(left, above, right, below, std::array<float, 4>{ shown_at(pane, left, top), shown_at(pane, right, top),
                                                             shown_at(pane, right, bottom), shown_at(pane, left, bottom) });
      }
    }
  };

  for (size_t index = first; index < m_requests.size(); ++index)
  {
    const DrawingRequest* request = m_requests[index];
    if (!passes(filter, *request))
    {
      continue;
    }
    pieces.srcrects.clear();
    pieces.dstrects.clear();
    pieces.angles.clear();
    pieces.tints.clear();

    if (request->type == TEXTURE)
    {
      // Another shiny picture isn't part of what's reflected, as with OpenGL 3.3
      const auto& source = static_cast<const TextureRequest&>(*request);
      if (source.displacement_texture || !source.shines.empty() || !source.texture)
      {
        continue;
      }
      pieces.texture = source.texture;
      pieces.flip = source.flip ^ VERTICAL_FLIP;
      pieces.alpha = source.alpha;
      pieces.blend = source.blend;
      pieces.color = source.color;
      pieces.between_pixels = source.between_pixels;
      for (size_t j = 0; j < source.dstrects.size(); ++j)
      {
        if (source.angles[j] != 0.0f || !near(source.dstrects[j]))
        {
          continue;
        }
        const Rectf& from = source.dstrects[j];
        reflect(index, from, [&](float left, float top, float right, float bottom, const std::array<float, 4>& shown) {
          pieces.srcrects.push_back(part_of(source.srcrects[j], from, source.flip, left, top, right, bottom));
          pieces.tints.push_back({ Color(1.0f, 1.0f, 1.0f, shown[0]), Color(1.0f, 1.0f, 1.0f, shown[1]),
                                   Color(1.0f, 1.0f, 1.0f, shown[2]), Color(1.0f, 1.0f, 1.0f, shown[3]) });
        });
      }
    }
    else if (request->type == GRADIENT)
    {
      const auto& gradient = static_cast<const GradientRequest&>(*request);
      const Rectf& region = gradient.region;
      const bool vertical = gradient.direction == VERTICAL || gradient.direction == VERTICAL_SECTOR;
      const auto color_at = [&](float x, float y, float strength) {
        const float along = vertical ? (y - region.get_top()) / region.get_height() : (x - region.get_left()) / region.get_width();
        const float t = std::clamp(along, 0.0f, 1.0f);
        return Color(gradient.top.red + (gradient.bottom.red - gradient.top.red) * t,
                     gradient.top.green + (gradient.bottom.green - gradient.top.green) * t,
                     gradient.top.blue + (gradient.bottom.blue - gradient.top.blue) * t,
                     (gradient.top.alpha + (gradient.bottom.alpha - gradient.top.alpha) * t) * strength);
      };
      pieces.texture = nullptr;
      pieces.flip = NO_FLIP;
      pieces.alpha = 1.0f;
      pieces.blend = gradient.blend;
      pieces.color = Color(1.0f, 1.0f, 1.0f);
      pieces.between_pixels = mirror.between_pixels;
      reflect(index, region, [&](float left, float top, float right, float bottom, const std::array<float, 4>& shown) {
        // A corner of the piece shows the gradient where it was before it turned upside down
        pieces.srcrects.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
        pieces.tints.push_back({ color_at(left, bottom, shown[0]), color_at(right, bottom, shown[1]),
                                 color_at(right, top, shown[2]), color_at(left, top, shown[3]) });
      });
    }

    if (!pieces.dstrects.empty())
    {
      painter.draw_texture(pieces);
    }
  }
}

void
Canvas::draw_surface(const SurfacePtr& surface,
                     const Vector& position, float angle, const Color& color, const Blend& blend,
                     int layer)
{
  if (!surface) return;

  const auto& cliprect = m_context.get_cliprect();

  // discard clipped surface
  if (position.x > cliprect.get_right() ||
     position.y > cliprect.get_bottom() ||
     position.x + static_cast<float>(surface->get_width()) < cliprect.get_left() ||
     position.y + static_cast<float>(surface->get_height()) < cliprect.get_top())
    return;

  auto request = new(m_arena) TextureRequest();

  request->type = TEXTURE;
  request->layer = layer;
  request->flip = m_context.transform().flip ^ surface->get_flip();
  request->alpha = m_context.transform().alpha;
  request->blend = blend;

  request->srcrects.emplace_back(Rectf(surface->get_region()));
  request->between_pixels = between_pixels();
  request->fades.push_back(m_context.transform().fade_edges ? FADE_ALL : 0);
  request->dstrects.emplace_back(place(position, Sizef(static_cast<float>(surface->get_width()),
                                                      static_cast<float>(surface->get_height()))));
  request->angles.emplace_back(angle);
  request->texture = surface->get_texture().get();
  request->displacement_texture = surface->get_displacement_texture().get();
  if (const Reflection* reflection = surface->get_reflection())
  {
    // Turned, it shows none
    request->shines.push_back(Shine{ angle == 0.0f ? reflection : nullptr, request->srcrects[0], request->dstrects[0] });
  }
  request->color = color;

  m_requests.push_back(request);
}

void
Canvas::draw_surface(const SurfacePtr& surface, const Vector& position, int layer)
{
  draw_surface(surface, position, 0.0f, Color(1.0f, 1.0f, 1.0f), Blend(), layer);
}

void
Canvas::draw_surface_scaled(const SurfacePtr& surface, const Rectf& dstrect,
                            int layer, const PaintStyle& style)
{
  draw_surface_part(surface, Rectf(0.0f, 0.0f, static_cast<float>(surface->get_width()), static_cast<float>(surface->get_height())),
                    dstrect, layer, style);
}

void
Canvas::draw_surface_part(const SurfacePtr& surface, const Rectf& srcrect, const Rectf& dstrect,
                          int layer, const PaintStyle& style)
{
  if (!surface) return;

  auto request = new(m_arena) TextureRequest();

  request->type = TEXTURE;
  request->layer = layer;
  request->flip = m_context.transform().flip ^ surface->get_flip();
  request->alpha = m_context.transform().alpha * style.get_alpha();
  request->blend = style.get_blend();

  request->srcrects.emplace_back(srcrect);
  request->between_pixels = between_pixels();
  request->fades.push_back(m_context.transform().fade_edges ? FADE_ALL : 0);
  request->dstrects.emplace_back(place(dstrect.p1(), dstrect.get_size()));
  request->angles.emplace_back(0.0f);
  request->texture = surface->get_texture().get();
  request->displacement_texture = surface->get_displacement_texture().get();
  if (const Reflection* reflection = surface->get_reflection())
  {
    request->shines.push_back(Shine{ reflection, request->srcrects[0], request->dstrects[0] });
  }
  request->color = style.get_color();

  m_requests.push_back(request);
}

void
Canvas::draw_surface_batch(const SurfacePtr& surface,
                           std::vector<Rectf> srcrects,
                           std::vector<Rectf> dstrects,
                           const Color& color,
                           int layer,
                           std::vector<uint8_t> fades,
                           std::vector<Shine> shines)
{
  const size_t len = srcrects.size();
  draw_surface_batch(surface,
                     std::move(srcrects),
                     std::move(dstrects),
                     std::vector<float>(len, 0.0f),
                     color, layer, std::move(fades), std::move(shines));
}

void
Canvas::draw_surface_batch(const SurfacePtr& surface,
                           std::vector<Rectf> srcrects,
                           std::vector<Rectf> dstrects,
                           std::vector<float> angles,
                           const Color& color,
                           int layer,
                           std::vector<uint8_t> fades,
                           std::vector<Shine> shines)
{
  if (!surface) return;

  auto request = new(m_arena) TextureRequest();

  request->type = TEXTURE;
  request->layer = layer;
  request->flip = m_context.transform().flip ^ surface->get_flip();
  request->alpha = m_context.transform().alpha;
  request->color = color;

  request->srcrects = std::move(srcrects);
  request->dstrects = std::move(dstrects);
  request->angles = std::move(angles);

  request->between_pixels = between_pixels();
  request->fades = std::move(fades);
  request->part_texels = m_context.transform().part_texels;
  const Placing taken = placing();
  for (auto& dstrect : request->dstrects)
  {
    dstrect = place(taken, dstrect.p1(), dstrect.get_size());
  }

  request->texture = surface->get_texture().get();
  request->displacement_texture = surface->get_displacement_texture().get();
  if (!shines.empty())
  {
    for (auto& shine : shines)
    {
      shine.dstrect = place(taken, shine.dstrect.p1(), shine.dstrect.get_size());
    }
    request->shines = std::move(shines);
  }
  else if (const Reflection* reflection = surface->get_reflection())
  {
    for (size_t i = 0; i < request->dstrects.size(); ++i)
    {
      request->shines.push_back(Shine{ request->angles[i] == 0.0f ? reflection : nullptr,
                                       request->srcrects[i], request->dstrects[i] });
    }
  }

  m_requests.push_back(request);
}

void
Canvas::draw_text(const FontPtr& font, const std::string& text,
                  const Vector& pos, FontAlignment alignment, int layer, const Color& color)
{
  font->draw_text(*this, text, pos, alignment, layer, color);
}

void
Canvas::draw_center_text(const FontPtr& font, const std::string& text,
                         const Vector& position, int layer, const Color& color)
{
  draw_text(font, text, Vector(position.x + static_cast<float>(m_context.get_width()) / 2.0f, position.y),
            ALIGN_CENTER, layer, color);
}

void
Canvas::draw_gradient(const Color& top, const Color& bottom, int layer,
                      const GradientDirection& direction, const Rectf& region,
                      const Blend& blend)
{
  auto request = new(m_arena) GradientRequest();

  request->type = GRADIENT;
  request->layer = layer;

  request->flip = m_context.transform().flip;
  request->alpha = m_context.transform().alpha;
  request->blend = blend;

  request->top = top;
  request->bottom = bottom;
  request->direction = direction;
  request->region = Rectf(apply_translate(region.p1())*scale(),
                          apply_translate(region.p2())*scale());

  m_requests.push_back(request);
}

void
Canvas::draw_filled_rect(const Rectf& rect, const Color& color,
                         int layer)
{
  draw_filled_rect(rect, color, 0.0f, layer);
}

void
Canvas::draw_filled_rect(const Rectf& rect, const Color& color, float radius, int layer)
{
  auto request = new(m_arena) FillRectRequest;

  request->type   = FILLRECT;
  request->layer  = layer;

  request->flip = m_context.transform().flip;
  request->alpha = m_context.transform().alpha;

  request->rect = Rectf(apply_translate(rect.p1())*scale(),
                        rect.get_size()*scale());
  request->color = color;
  request->color.alpha = color.alpha * m_context.transform().alpha;
  request->radius = radius;
  request->whole_pixels = on_pixel_grid();

  m_requests.push_back(request);
}

void
Canvas::draw_inverse_ellipse(const Vector& pos, const Vector& size, const Color& color, int layer)
{
  auto request = new(m_arena) InverseEllipseRequest;

  request->type   = INVERSEELLIPSE;
  request->layer  = layer;

  request->flip = m_context.transform().flip;
  request->alpha = m_context.transform().alpha;

  request->pos          = apply_translate(pos)*scale();
  request->color        = color;
  request->color.alpha  = color.alpha * m_context.transform().alpha;
  request->size         = size*scale();

  m_requests.push_back(request);
}

void
Canvas::draw_line(const Vector& pos1, const Vector& pos2, const Color& color, int layer)
{
  auto request = new(m_arena) LineRequest;

  request->type   = LINE;
  request->layer  = layer;

  request->flip = m_context.transform().flip;
  request->alpha = m_context.transform().alpha;

  request->pos          = apply_translate(pos1)*scale();
  request->color        = color;
  request->color.alpha  = color.alpha * m_context.transform().alpha;
  request->dest_pos     = apply_translate(pos2)*scale();

  m_requests.push_back(request);
}

void
Canvas::draw_triangle(const Vector& pos1, const Vector& pos2, const Vector& pos3, const Color& color, int layer)
{
  auto request = new(m_arena) TriangleRequest;

  request->type   = TRIANGLE;
  request->layer  = layer;

  request->flip = m_context.transform().flip;
  request->alpha = m_context.transform().alpha;

  request->pos1 = apply_translate(pos1)*scale();
  request->pos2 = apply_translate(pos2)*scale();
  request->pos3 = apply_translate(pos3)*scale();
  request->color = color;
  request->color.alpha = color.alpha * m_context.transform().alpha;

  m_requests.push_back(request);
}

void
Canvas::get_pixel(const Vector& position, const std::shared_ptr<Color>& color_out)
{
  assert(color_out);

  Vector pos = apply_translate(position)*scale();

  // There is no light offscreen.
  if (pos.x >= static_cast<float>(m_context.get_viewport().get_width()) ||
      pos.y >= static_cast<float>(m_context.get_viewport().get_height()) ||
      pos.x < 0.0f ||
      pos.y < 0.0f)
  {
    *color_out = Color(0, 0, 0);
    return;
  }

  auto request = new(m_arena) GetPixelRequest();

  request->layer = LAYER_GETPIXEL;
  request->pos = pos;
  request->color_ptr = color_out;

  m_requests.push_back(request);
}

Canvas::Placing
Canvas::placing() const
{
  Placing result;
  result.translation = m_context.transform().translation;
  result.offset = Vector(static_cast<float>(m_context.get_viewport().left), static_cast<float>(m_context.get_viewport().top));
  result.scale = scale();
  result.grid = on_pixel_grid();
  result.pixels = result.grid ? VideoSystem::current()->get_viewport().get_scale() * result.scale : Vector(0.0f, 0.0f);
  return result;
}

Vector
Canvas::apply_translate(const Placing& placing, const Vector& pos)
{
  if (placing.grid && placing.pixels.x > 0.0f && placing.pixels.y > 0.0f)
  {
    // Rounded apart, so where the camera is never changes how a picture rounds
    return (glm::round(pos * placing.pixels) - glm::round(placing.translation * placing.pixels)) / placing.pixels + placing.offset;
  }

  return (pos - placing.translation) + placing.offset;
}

Vector
Canvas::apply_translate(const Vector& pos) const
{
  return apply_translate(placing(), pos);
}

Rectf
Canvas::place(const Placing& placing, const Vector& pos, const Sizef& size)
{
  if (placing.grid)
  {
    return Rectf(apply_translate(placing, pos) * placing.scale,
                 apply_translate(placing, pos + Vector(size.width, size.height)) * placing.scale);
  }

  return Rectf(apply_translate(placing, pos) * placing.scale, size * placing.scale);
}

Rectf
Canvas::place(const Vector& pos, const Sizef& size) const
{
  return place(placing(), pos, size);
}

bool
Canvas::between_pixels() const
{
  return m_context.transform().between_pixels && VideoSystem::current()->draws_between_pixels();
}

bool
Canvas::on_pixel_grid() const
{
  return m_context.transform().pixel_grid && !between_pixels();
}

float
Canvas::scale() const
{
  return m_context.transform().scale;
}

/* EOF */
