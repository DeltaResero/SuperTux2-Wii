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

#include "video/sdl/sdl_painter.hpp"

#include <SDL.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>

#include "supertux/globals.hpp"
#include "math/util.hpp"
#include "util/log.hpp"
#include "video/drawing_request.hpp"
#include "video/renderer.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_video_system.hpp"
#include "video/viewport.hpp"

namespace {

SDL_Rect to_sdl_rect(const Rectf& rect)
{
  SDL_Rect sdl_rect;

  // floorf() here due to int(-0.5) and int(0.5) both rounding to 0,
  // thus creating a jump in coordinates at 0
  sdl_rect.x = static_cast<int>(floorf(rect.get_left()));
  sdl_rect.y = static_cast<int>(floorf(rect.get_top()));

  // roundf() here due to int(rect.get_right()y - rect.get_left()y) being
  // off-by-one due to float errors
  sdl_rect.w = static_cast<int>(roundf(rect.get_width()));
  sdl_rect.h = static_cast<int>(roundf(rect.get_height()));

  return sdl_rect;
}

SDL_FRect to_sdl_frect(const SDL_Rect& rect, float scale)
{
  return SDL_FRect{ static_cast<float>(rect.x) * scale, static_cast<float>(rect.y) * scale,
                    static_cast<float>(rect.w) * scale, static_cast<float>(rect.h) * scale };
}

SDL_BlendMode blend2sdl(const Blend& blend)
{
  if (blend == Blend::NONE)
  {
    return SDL_BLENDMODE_NONE;
  }
  else if (blend == Blend::BLEND)
  {
    return SDL_BLENDMODE_BLEND;
  }
  else if (blend == Blend::ADD)
  {
    return SDL_BLENDMODE_ADD;
  }
  else if (blend == Blend::MOD)
  {
    return SDL_BLENDMODE_MOD;
  }
  else
  {
    log_warning << "unknown blend mode combinations: blend=" << static_cast<int>(blend) << std::endl;
    return SDL_BLENDMODE_BLEND;
  }
}

/* Creates a new rectangle covering the area where srcrect and imgrect
   overlap, in addition create four more rectangles for the areas
   where srcrect is outside of imgrect, some of those rects will be
   empty. The rectangles will be returned in the order inside, top,
   left, right, bottom */
std::tuple<Rect, Rect, Rect, Rect, Rect>
intersect(const Rect& srcrect, const Rect& imgrect)
{
  return std::make_tuple(
    // inside
    Rect(std::max(srcrect.left, imgrect.left), std::max(srcrect.top, imgrect.top),
         std::min(srcrect.right, imgrect.right), std::min(srcrect.bottom, imgrect.bottom)),

    // top
    Rect(srcrect.left, srcrect.top,
         srcrect.right, imgrect.top),

    // left
    Rect(srcrect.left, std::max(srcrect.top, imgrect.top),
         imgrect.left, std::min(srcrect.bottom, imgrect.bottom)),

    // right
    Rect(imgrect.right, std::max(srcrect.top, imgrect.top),
         srcrect.right, std::min(srcrect.bottom, imgrect.bottom)),

    // bottom
    Rect(srcrect.left, imgrect.bottom,
         srcrect.right, srcrect.bottom)
    );
}

/* Map the area covered by inside in srcrect to dstrect */
SDL_FRect relative_map(const Rect& inside, const Rect& srcrect, const SDL_FRect& dstrect)
{
  assert(srcrect.contains(inside));

  const float scale_x = dstrect.w / static_cast<float>(srcrect.get_width());
  const float scale_y = dstrect.h / static_cast<float>(srcrect.get_height());

  return SDL_FRect{ dstrect.x + static_cast<float>(inside.left - srcrect.left) * scale_x,
                    dstrect.y + static_cast<float>(inside.top - srcrect.top) * scale_y,
                    static_cast<float>(inside.get_width()) * scale_x,
                    static_cast<float>(inside.get_height()) * scale_y };
}

void render_texture(SDL_Renderer* renderer,
                    SDL_Texture* texture, const Rect& imgrect,
                    const Rect& srcrect, const SDL_FRect& dstrect)
{
  assert(imgrect.contains(srcrect.left, srcrect.top));

  if (srcrect.empty() || dstrect.w <= 0.0f || dstrect.h <= 0.0f)
    return;

  if (imgrect.contains(srcrect))
  {
    SDL_Rect sdl_srcrect = srcrect.to_sdl();
    SDL_RenderCopyF(renderer, texture, &sdl_srcrect, &dstrect);
  }
  else
  {
    Rect inside;
    std::array<Rect, 4> rest;
    std::tie(inside, rest[0], rest[1], rest[2], rest[3]) = intersect(srcrect, imgrect);

    render_texture(renderer, texture, imgrect, inside, relative_map(inside, srcrect, dstrect));

    for (const Rect& rect : rest)
    {
      const Rect new_srcrect(math::positive_mod(rect.left, imgrect.get_width()),
                             math::positive_mod(rect.top, imgrect.get_height()),
                             rect.get_size());
      render_texture(renderer, texture, imgrect,
                     new_srcrect, relative_map(rect, srcrect, dstrect));
    }
  }
}

/** Draws the picture with its FADE_ sides fading out over 1 screen pixel, so a pixel on the edge shows as much as it covers */
void RenderCopyFaded(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& src, const SDL_FRect& dst,
                     uint8_t fades, SDL_RendererFlip flip, SDL_Color color)
{
  int texture_width = 0;
  int texture_height = 0;
  SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height);
  Vector scale;
  SDL_RenderGetScale(renderer, &scale.x, &scale.y);
  const Vector half(0.5f / scale.x, 0.5f / scale.y);

  float u0 = static_cast<float>(src.x) / static_cast<float>(texture_width);
  float u1 = static_cast<float>(src.x + src.w) / static_cast<float>(texture_width);
  float v0 = static_cast<float>(src.y) / static_cast<float>(texture_height);
  float v1 = static_cast<float>(src.y + src.h) / static_cast<float>(texture_height);
  // A side cut from inside a bigger texture doesn't fade, or it would show what lies beside it
  bool cut_left = src.x > 0, cut_right = src.x + src.w < texture_width;
  bool cut_top = src.y > 0, cut_bottom = src.y + src.h < texture_height;
  if (flip & SDL_FLIP_HORIZONTAL) { std::swap(u0, u1); std::swap(cut_left, cut_right); }
  if (flip & SDL_FLIP_VERTICAL) { std::swap(v0, v1); std::swap(cut_top, cut_bottom); }

  const bool wide = dst.w > 4.0f * half.x, tall = dst.h > 4.0f * half.y;
  const auto xs = Painter::fade_lines(dst.x, dst.x + dst.w, half.x,
                                      wide && (fades & FADE_LEFT) && !cut_left, wide && (fades & FADE_RIGHT) && !cut_right);
  const auto ys = Painter::fade_lines(dst.y, dst.y + dst.h, half.y,
                                      tall && (fades & FADE_TOP) && !cut_top, tall && (fades & FADE_BOTTOM) && !cut_bottom);

  std::vector<SDL_Vertex> vertices;
  for (const auto& [y, shown_y] : ys)
  {
    for (const auto& [x, shown_x] : xs)
    {
      SDL_Vertex vertex;
      vertex.position = SDL_FPoint{ x, y };
      vertex.color = color;
      vertex.color.a = static_cast<Uint8>(std::lround(static_cast<float>(color.a) * shown_x * shown_y));
      // Held inside the picture, as SDL may wrap a point past the texture's edge round to the far side
      vertex.tex_coord = SDL_FPoint{ u0 + std::clamp((x - dst.x) / dst.w, 0.0f, 1.0f) * (u1 - u0),
                                     v0 + std::clamp((y - dst.y) / dst.h, 0.0f, 1.0f) * (v1 - v0) };
      vertices.push_back(vertex);
    }
  }

  std::vector<int> indices;
  const int columns = static_cast<int>(xs.size());
  for (int row = 0; row + 1 < static_cast<int>(ys.size()); ++row)
  {
    for (int column = 0; column + 1 < columns; ++column)
    {
      const int corner = row * columns + column;
      for (int index : { corner, corner + 1, corner + columns + 1, corner, corner + columns + 1, corner + columns })
      {
        indices.push_back(index);
      }
    }
  }

  SDL_RenderGeometry(renderer, texture, vertices.data(), static_cast<int>(vertices.size()),
                     indices.data(), static_cast<int>(indices.size()));
}

/** Draws every rectangle at once, from texel rectangles that may start or end inside a texel where SDL_RenderCopy can't */
void RenderCopyParts(SDL_Renderer* renderer, SDL_Texture* texture, const std::vector<Rectf>& texels,
                     const std::vector<SDL_FRect>& dsts, SDL_Color color)
{
  int texture_width = 0;
  int texture_height = 0;
  SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height);
  const float width = static_cast<float>(texture_width);
  const float height = static_cast<float>(texture_height);

  // Written in place, as a push per corner costs more than the drawing in a build without optimising
  std::vector<SDL_Vertex> vertices(texels.size() * 4);
  std::vector<int> indices(texels.size() * 6);
  for (size_t i = 0; i < texels.size(); ++i)
  {
    const float u0 = texels[i].get_left() / width;
    const float u1 = texels[i].get_right() / width;
    const float v0 = texels[i].get_top() / height;
    const float v1 = texels[i].get_bottom() / height;
    const SDL_FRect& dst = dsts[i];
    SDL_Vertex* vertex = &vertices[i * 4];
    vertex[0] = { SDL_FPoint{ dst.x, dst.y }, color, SDL_FPoint{ u0, v0 } };
    vertex[1] = { SDL_FPoint{ dst.x + dst.w, dst.y }, color, SDL_FPoint{ u1, v0 } };
    vertex[2] = { SDL_FPoint{ dst.x + dst.w, dst.y + dst.h }, color, SDL_FPoint{ u1, v1 } };
    vertex[3] = { SDL_FPoint{ dst.x, dst.y + dst.h }, color, SDL_FPoint{ u0, v1 } };
    const int corner = static_cast<int>(i * 4);
    int* index = &indices[i * 6];
    index[0] = corner;
    index[1] = corner + 1;
    index[2] = corner + 2;
    index[3] = corner;
    index[4] = corner + 2;
    index[5] = corner + 3;
  }
  SDL_RenderGeometry(renderer, texture, vertices.data(), static_cast<int>(vertices.size()),
                     indices.data(), static_cast<int>(indices.size()));
}

/* A version SDL_RenderCopyEx that supports texture animation as specified by Sampler */
void RenderCopyEx(SDL_Renderer*          renderer,
                  SDL_Texture*           texture,
                  const SDL_Rect*        sdl_srcrect,
                  const SDL_FRect*       sdl_dstrect,
                  const double           angle,
                  const SDL_Point*       center,
                  const SDL_RendererFlip flip,
                  const Sampler& sampler,
                  const Vector& texel_scale)
{
  Vector animate = sampler.get_animate() * texel_scale;
  if (animate.x == 0.0f && animate.y == 0.0f)
  {
    SDL_RenderCopyExF(renderer, texture, sdl_srcrect, sdl_dstrect, angle, nullptr, flip);
  }
  else
  {
    // This part deals with texture animation. Texture animation is
    // accomplished by shifting the srcrect across the input texture.
    // If the srcrect goes out of bounds of the texture, it is broken
    // up into multiple rectangles that wrap around and fall back into
    // the texture space.
    //
    // If a srcrect is passed to SDL that goes out of bounds SDL will
    // clip it to be inside the bounds, without adjusting dstrect,
    // thus result in stretching artifacts.
    //
    // FIXME: Neither flipping nor wrap modes are supported at the
    // moment. wrap is treated as if it was set to 'repeat'.
    int width;
    int height;

    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

    animate *= g_game_time;

    int tex_off_x = math::positive_mod(static_cast<int>(animate.x), width);
    int tex_off_y = math::positive_mod(static_cast<int>(animate.y), height);

    if ((tex_off_x == 0 && tex_off_y == 0) ||
        flip ||
        angle != 0.0)
    {
      SDL_RenderCopyExF(renderer, texture, sdl_srcrect, sdl_dstrect, angle, nullptr, flip);
    }
    else
    {
      Rect imgrect(0, 0, Size(width, height));
      Rect srcrect(math::positive_mod(sdl_srcrect->x + tex_off_x, width),
                   math::positive_mod(sdl_srcrect->y + tex_off_y, height),
                   Size(sdl_srcrect->w, sdl_srcrect->h));

      render_texture(renderer, texture, imgrect, srcrect, *sdl_dstrect);
    }
  }
}

} // namespace

SDLPainter::SDLPainter(SDLVideoSystem& video_system, Renderer& renderer, SDL_Renderer* sdl_renderer, float scale) :
  m_video_system(video_system),
  m_renderer(renderer),
  m_sdl_renderer(sdl_renderer),
  m_scale(scale),
  m_cliprect()
{}

void
SDLPainter::draw_texture(const TextureRequest& request)
{
  const auto& texture = static_cast<const SDLTexture&>(*request.texture);

  assert(request.srcrects.size() == request.dstrects.size());
  assert(request.srcrects.size() == request.angles.size());

  if (request.part_texels && request.flip == NO_FLIP && texture.get_sampler().get_animate() == Vector(0.0f, 0.0f) &&
      std::all_of(request.angles.begin(), request.angles.end(), [](float angle) { return angle == 0.0f; }) &&
      std::all_of(request.fades.begin(), request.fades.end(), [](uint8_t fades) { return fades == 0; }))
  {
    const Vector& texel_scale = texture.get_texel_scale();
    std::vector<Rectf> texels(request.srcrects.size());
    std::vector<SDL_FRect> dsts(request.srcrects.size());
    for (size_t i = 0; i < request.srcrects.size(); ++i)
    {
      const Rectf& src = request.srcrects[i];
      texels[i] = Rectf(src.get_left() * texel_scale.x, src.get_top() * texel_scale.y,
                        src.get_right() * texel_scale.x, src.get_bottom() * texel_scale.y);
      dsts[i] = to_target(request.dstrects[i], request.between_pixels);
    }
    // The colour goes in with the corners, so the texture's own tint is left plain
    SDL_SetTextureColorMod(texture.get_texture(), 255, 255, 255);
    SDL_SetTextureAlphaMod(texture.get_texture(), 255);
    SDL_SetTextureBlendMode(texture.get_texture(), blend2sdl(request.blend));
    RenderCopyParts(m_sdl_renderer, texture.get_texture(), texels, dsts,
                    SDL_Color{ static_cast<Uint8>(request.color.red * 255), static_cast<Uint8>(request.color.green * 255),
                               static_cast<Uint8>(request.color.blue * 255),
                               static_cast<Uint8>(request.color.alpha * request.alpha * 255) });
    return;
  }

  for (size_t i = 0; i < request.srcrects.size(); ++i)
  {
    // The texture may hold fewer pixels than the picture it stands for
    const Rectf& src = request.srcrects[i];
    const Vector& texel_scale = texture.get_texel_scale();
    const SDL_Rect& src_rect = to_sdl_rect(Rectf(src.get_left() * texel_scale.x, src.get_top() * texel_scale.y,
                                                 src.get_right() * texel_scale.x, src.get_bottom() * texel_scale.y));
    const SDL_FRect dst_rect = to_target(request.dstrects[i], request.between_pixels);

    Uint8 r = static_cast<Uint8>(request.color.red * 255);
    Uint8 g = static_cast<Uint8>(request.color.green * 255);
    Uint8 b = static_cast<Uint8>(request.color.blue * 255);
    Uint8 a = static_cast<Uint8>(request.color.alpha * request.alpha * 255);

    SDL_SetTextureColorMod(texture.get_texture(), r, g, b);
    SDL_SetTextureAlphaMod(texture.get_texture(), a);
    SDL_SetTextureBlendMode(texture.get_texture(), blend2sdl(request.blend));

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if ((request.flip & HORIZONTAL_FLIP) != 0)
    {
      flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
    }

    if ((request.flip & VERTICAL_FLIP) != 0)
    {
      flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
    }

    const uint8_t fades = (i < request.fades.size()) ? request.fades[i] : 0;
    if (fades != 0 && request.between_pixels && request.angles[i] == 0.0f && !SDL_GetRenderTarget(m_sdl_renderer) &&
        texture.get_sampler().get_animate() == Vector(0.0f, 0.0f))
    {
      // The colour goes in with the corners, so the texture's own tint is left plain
      SDL_SetTextureColorMod(texture.get_texture(), 255, 255, 255);
      SDL_SetTextureAlphaMod(texture.get_texture(), 255);
      RenderCopyFaded(m_sdl_renderer, texture.get_texture(), src_rect, dst_rect, fades, flip, SDL_Color{ r, g, b, a });
    }
    else
    {
      RenderCopyEx(m_sdl_renderer, texture.get_texture(),
                   &src_rect, &dst_rect,
                   static_cast<double>(request.angles[i]), nullptr, flip,
                   texture.get_sampler(), texel_scale);
    }
  }
}

SDL_FRect
SDLPainter::to_target(const Rectf& rect, bool between_pixels) const
{
  // Fractions kept for a gliding layer, and on the lightmap, where whole pixels would move a light in steps
  if (SDL_GetRenderTarget(m_sdl_renderer) || between_pixels)
  {
    return SDL_FRect{ rect.get_left() * m_scale, rect.get_top() * m_scale,
                      rect.get_width() * m_scale, rect.get_height() * m_scale };
  }

  // Both edges go to the nearest whole screen pixel, as an edge meant for the last one can come out a hair short
  const Viewport& viewport = m_video_system.get_viewport();
  Vector scale;
  SDL_RenderGetScale(m_sdl_renderer, &scale.x, &scale.y);
  const float origin_x = std::floor(static_cast<float>(viewport.get_rect().left) / scale.x);
  const float origin_y = std::floor(static_cast<float>(viewport.get_rect().top) / scale.y);
  const float left = std::round((origin_x + rect.get_left() * m_scale) * scale.x);
  const float top = std::round((origin_y + rect.get_top() * m_scale) * scale.y);
  const float right = std::round((origin_x + rect.get_right() * m_scale) * scale.x);
  const float bottom = std::round((origin_y + rect.get_bottom() * m_scale) * scale.y);
  return SDL_FRect{ left / scale.x - origin_x, top / scale.y - origin_y,
                    (right - left) / scale.x, (bottom - top) / scale.y };
}

void
SDLPainter::fill_whole_pixels(const Rectf& rect, float radius) const
{
  // Edges on whole screen pixels as to_target puts pictures', corners rounded row by row of whole pixels
  const Viewport& viewport = m_video_system.get_viewport();
  Vector scale;
  SDL_RenderGetScale(m_sdl_renderer, &scale.x, &scale.y);
  const float origin_x = std::floor(static_cast<float>(viewport.get_rect().left) / scale.x);
  const float origin_y = std::floor(static_cast<float>(viewport.get_rect().top) / scale.y);
  const int left = static_cast<int>(std::round((origin_x + rect.get_left() * m_scale) * scale.x));
  const int top = static_cast<int>(std::round((origin_y + rect.get_top() * m_scale) * scale.y));
  const int right = static_cast<int>(std::round((origin_x + rect.get_right() * m_scale) * scale.x));
  const int bottom = static_cast<int>(std::round((origin_y + rect.get_bottom() * m_scale) * scale.y));
  if (right <= left || bottom <= top) return;

  const float radius_x = std::min(radius * m_scale * scale.x, static_cast<float>(right - left) / 2.0f);
  const float radius_y = std::min(radius * m_scale * scale.y, static_cast<float>(bottom - top) / 2.0f);
  const int rows = static_cast<int>(radius_y);
  std::vector<SDL_FRect> rects;
  const auto add = [&](int x0, int y0, int x1, int y1) {
    rects.push_back(SDL_FRect{ static_cast<float>(x0) / scale.x - origin_x, static_cast<float>(y0) / scale.y - origin_y,
                               static_cast<float>(x1 - x0) / scale.x, static_cast<float>(y1 - y0) / scale.y });
  };
  for (int i = 0; i < rows; ++i)
  {
    const float dy = (radius_y - (static_cast<float>(i) + 0.5f)) / radius_y;
    const int inset = static_cast<int>(std::round(radius_x - std::sqrt(std::max(0.0f, 1.0f - dy * dy)) * radius_x));
    add(left + inset, top + i, right - inset, top + i + 1);
    add(left + inset, bottom - 1 - i, right - inset, bottom - i);
  }
  if (bottom - top > 2 * rows)
  {
    add(left, top + rows, right, bottom - rows);
  }
  SDL_RenderFillRectsF(m_sdl_renderer, rects.data(), static_cast<int>(rects.size()));
}

void
SDLPainter::draw_gradient(const GradientRequest& request)
{
  const Color& top = request.top;
  const Color& bottom = request.bottom;
  const GradientDirection& direction = request.direction;
  const Rectf& region = request.region;

  // calculate the maximum number of steps needed for the gradient
  int n = static_cast<int>(std::max(std::max(fabsf(top.red - bottom.red),
                                             fabsf(top.green - bottom.green)),
                                    std::max(fabsf(top.blue - bottom.blue),
                                             fabsf(top.alpha - bottom.alpha))) * 255);
  n = std::max(n, 1);
  for (int i = 0; i < n; ++i)
  {
    SDL_Rect rect;
    if (direction == VERTICAL || direction == VERTICAL_SECTOR)
    {
      rect.x = static_cast<int>(region.get_left());
      rect.y = static_cast<int>(region.get_bottom() * static_cast<float>(i) / static_cast<float>(n));
      rect.w = static_cast<int>(region.get_right());
      rect.h = static_cast<int>((region.get_bottom() * static_cast<float>(i+1) / static_cast<float>(n)) - static_cast<float>(rect.y));
    }
    else
    {
      rect.x = static_cast<int>(region.get_right() * static_cast<float>(i) / static_cast<float>(n));
      rect.y = static_cast<int>(region.get_top());
      rect.w = static_cast<int>((region.get_right() * static_cast<float>(i+1) / static_cast<float>(n)) - static_cast<float>(rect.x));
      rect.h = static_cast<int>(region.get_bottom());
    }

    float p = static_cast<float>(i+1) / static_cast<float>(n);
    Uint8 r, g, b, a;

    if ( direction == HORIZONTAL_SECTOR || direction == VERTICAL_SECTOR)
    {
        float begin_percentage = region.get_left() * -1 / region.get_right();
        r = static_cast<Uint8>(((1.0f - begin_percentage - p) * top.red + (p + begin_percentage) * bottom.red)  * 255);
        g = static_cast<Uint8>(((1.0f - begin_percentage - p) * top.green + (p + begin_percentage) * bottom.green) * 255);
        b = static_cast<Uint8>(((1.0f - begin_percentage - p) * top.blue + (p + begin_percentage) * bottom.blue) * 255);
        a = static_cast<Uint8>(((1.0f - begin_percentage - p) * top.alpha + (p + begin_percentage) * bottom.alpha) * 255);
    }
    else
    {
        r = static_cast<Uint8>(((1.0f - p) * top.red + p * bottom.red)  * 255);
        g = static_cast<Uint8>(((1.0f - p) * top.green + p * bottom.green) * 255);
        b = static_cast<Uint8>(((1.0f - p) * top.blue + p * bottom.blue) * 255);
        a = static_cast<Uint8>(((1.0f - p) * top.alpha + p * bottom.alpha) * 255);
    }

    SDL_SetRenderDrawBlendMode(m_sdl_renderer, blend2sdl(request.blend));
    SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
    const SDL_FRect frect = to_sdl_frect(rect, m_scale);
    SDL_RenderFillRectF(m_sdl_renderer, &frect);
  }
}

void
SDLPainter::draw_filled_rect(const FillRectRequest& request)
{
  SDL_Rect rect = to_sdl_rect(request.rect);

  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  if (request.whole_pixels)
  {
    SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
    fill_whole_pixels(request.rect, request.radius);
    return;
  }

  int radius = std::min(std::min(rect.h / 2, rect.w / 2),
                        static_cast<int>(request.radius));

  if (radius)
  {
    int slices = radius;

    // rounded top and bottom parts
    std::vector<SDL_FRect> rects;
    rects.reserve(2*slices + 1);
    for (int i = 0; i < slices; ++i)
    {
      float p = (static_cast<float>(i) + 0.5f) / static_cast<float>(slices);
      int xoff = radius - static_cast<int>(sqrtf(1.0f - p * p) * static_cast<float>(radius));

      SDL_Rect tmp;
      tmp.x = rect.x + xoff;
      tmp.y = rect.y + (radius - i);
      tmp.w = rect.w - 2*(xoff);
      tmp.h = 1;
      rects.push_back(to_sdl_frect(tmp, m_scale));

      SDL_Rect tmp2;
      tmp2.x = rect.x + xoff;
      tmp2.y = rect.y + rect.h - radius + i;
      tmp2.w = rect.w - 2*xoff;
      tmp2.h = 1;

      if (tmp2.y != tmp.y)
      {
        rects.push_back(to_sdl_frect(tmp2, m_scale));
      }
    }

    if (2*radius < rect.h)
    {
      // center rectangle
      SDL_Rect tmp;
      tmp.x = rect.x;
      tmp.y = rect.y + radius + 1;
      tmp.w = rect.w;
      tmp.h = rect.h - 2*radius - 1;
      rects.push_back(to_sdl_frect(tmp, m_scale));
    }

    SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
    SDL_RenderFillRectsF(m_sdl_renderer, &*rects.begin(), static_cast<int>(rects.size()));
  }
  else
  {
    if ((rect.w != 0) && (rect.h != 0))
    {
      SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
      const SDL_FRect frect = to_sdl_frect(rect, m_scale);
      SDL_RenderFillRectF(m_sdl_renderer, &frect);
    }
  }
}

void
SDLPainter::draw_inverse_ellipse(const InverseEllipseRequest& request)
{
  float x = request.pos.x;
  float w = request.size.x;
  float h = request.size.y;

  int top = static_cast<int>(request.pos.y - (h / 2));

  const Viewport& viewport = m_video_system.get_viewport();

  const int max_slices = 256;
  SDL_Rect rects[2*max_slices+2];
  int slices = std::min(static_cast<int>(request.size.y), max_slices);
  for (int i = 0; i < slices; ++i)
  {
    float p = ((static_cast<float>(i) + 0.5f) / static_cast<float>(slices)) * 2.0f - 1.0f;
    int xoff = static_cast<int>(sqrtf(1.0f - p*p) * w / 2);

    SDL_Rect& left  = rects[2*i+0];
    SDL_Rect& right = rects[2*i+1];

    left.x = 0;
    left.y = top + (i * static_cast<int>(h) / slices);
    left.w = static_cast<int>(x) - xoff;
    left.h = top + ((i+1) * static_cast<int>(h) / slices) - left.y;

    right.x = static_cast<int>(x) + xoff;
    right.y = left.y;
    right.w = viewport.get_screen_width() - right.x;
    right.h = left.h;
  }

  SDL_Rect& top_rect = rects[2*slices+0];
  SDL_Rect& bottom_rect = rects[2*slices+1];

  top_rect.x = 0;
  top_rect.y = 0;
  top_rect.w = viewport.get_screen_width();
  top_rect.h = top;

  bottom_rect.x = 0;
  bottom_rect.y = top + static_cast<int>(h);
  bottom_rect.w = viewport.get_screen_width();
  bottom_rect.h = viewport.get_screen_height() - bottom_rect.y;

  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_FRect frects[2*max_slices+2];
  for (int i = 0; i < 2*slices+2; ++i)
    frects[i] = to_sdl_frect(rects[i], m_scale);

  SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
  SDL_RenderFillRectsF(m_sdl_renderer, frects, 2*slices+2);
}

void
SDLPainter::draw_line(const LineRequest& request)
{
  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  int x1 = static_cast<int>(request.pos.x);
  int y1 = static_cast<int>(request.pos.y);
  int x2 = static_cast<int>(request.dest_pos.x);
  int y2 = static_cast<int>(request.dest_pos.y);

  SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
  SDL_RenderDrawLineF(m_sdl_renderer,
                      static_cast<float>(x1) * m_scale, static_cast<float>(y1) * m_scale,
                      static_cast<float>(x2) * m_scale, static_cast<float>(y2) * m_scale);
}

namespace {

using Edge = std::pair<Vector, Vector>;

Edge
make_edge(int x1, int y1, int x2, int y2)
{
  if (y1 < y2)
  {
    return Edge(Vector(static_cast<float>(x1), static_cast<float>(y1)),
                Vector(static_cast<float>(x2), static_cast<float>(y2)));
  }
  else
  {
    return Edge(Vector(static_cast<float>(x2), static_cast<float>(y2)),
                Vector(static_cast<float>(x1), static_cast<float>(y1)));
  }
}

void
draw_span_between_edges(SDL_Renderer* renderer, const Edge& e1, const Edge& e2, float scale)
{
  // calculate difference between the y coordinates
  // of the first edge and return if 0
  float e1ydiff = static_cast<float>(e1.second.y - e1.first.y);
  if (e1ydiff == 0.0f)
    return;

  // calculate difference between the y coordinates
  // of the second edge and return if 0
  float e2ydiff = static_cast<float>(e2.second.y - e2.first.y);
  if (e2ydiff == 0.0f)
    return;

  float e1xdiff = e1.second.x - e1.first.x;
  float e2xdiff = e2.second.x - e2.first.x;
  float factor1 = (e2.first.y - e1.first.y) / e1ydiff;
  float factorStep1 = 1.0f / e1ydiff;
  float factor2 = 0.0f;
  float factorStep2 = 1.0f / e2ydiff;

  for (int y = static_cast<int>(e2.first.y); y < static_cast<int>(e2.second.y); y++) {
    SDL_RenderDrawLineF(renderer,
                        static_cast<float>(static_cast<int>(e1.first.x + e1xdiff * factor1)) * scale,
                        static_cast<float>(y) * scale,
                        static_cast<float>(static_cast<int>(e2.first.x + e2xdiff * factor2)) * scale,
                        static_cast<float>(y) * scale);
    factor1 += factorStep1;
    factor2 += factorStep2;
  }
}

} //namespace

void
SDLPainter::draw_triangle(const TriangleRequest& request)
{
  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  int x1 = static_cast<int>(request.pos1.x);
  int y1 = static_cast<int>(request.pos1.y);
  int x2 = static_cast<int>(request.pos2.x);
  int y2 = static_cast<int>(request.pos2.y);
  int x3 = static_cast<int>(request.pos3.x);
  int y3 = static_cast<int>(request.pos3.y);

  std::array<Edge, 3> edges{
    make_edge(x1, y1, x2, y2),
    make_edge(x2, y2, x3, y3),
    make_edge(x3, y3, x1, y1)
  };

  int maxLength = 0;
  int longEdge = 0;

  // find edge with the greatest length in the y axis
  for (int i = 0; i < 3; i++) {
    int length = static_cast<int>(edges[i].second.y - edges[i].first.y);
    if (length > maxLength) {
      maxLength = length;
      longEdge = i;
    }
  }
  int shortEdge1 = (longEdge + 1) % 3;
  int shortEdge2 = (longEdge + 2) % 3;

  SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);

  draw_span_between_edges(m_sdl_renderer, edges[longEdge], edges[shortEdge1], m_scale);
  draw_span_between_edges(m_sdl_renderer, edges[longEdge], edges[shortEdge2], m_scale);
}

void
SDLPainter::clear(const Color& color)
{
  SDL_SetRenderDrawColor(m_sdl_renderer, color.r8(), color.g8(), color.b8(), color.a8());

  if (m_cliprect)
  {
    SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_NONE);
    SDL_RenderFillRect(m_sdl_renderer, &*m_cliprect);
  }
  else
  {
    // This ignores the cliprect:
    SDL_RenderClear(m_sdl_renderer);
  }
}

void
SDLPainter::set_clip_rect(const Rect& rect)
{
  // Kept in the target's own pixels, rounded outwards
  const int left = static_cast<int>(floorf(static_cast<float>(rect.left) * m_scale));
  const int top = static_cast<int>(floorf(static_cast<float>(rect.top) * m_scale));
  const int right = static_cast<int>(ceilf(static_cast<float>(rect.right) * m_scale));
  const int bottom = static_cast<int>(ceilf(static_cast<float>(rect.bottom) * m_scale));
  m_cliprect = SDL_Rect{ left, top, right - left, bottom - top };

  int ret = SDL_RenderSetClipRect(m_sdl_renderer, &*m_cliprect);
  if (ret < 0)
  {
    log_warning << "SDLPainter::set_clip_rect(): SDL_RenderSetClipRect() failed: " << SDL_GetError() << std::endl;
  }
}

void
SDLPainter::clear_clip_rect()
{
  m_cliprect.reset();

  int ret = SDL_RenderSetClipRect(m_sdl_renderer, nullptr);
  if (ret < 0)
  {
    log_warning << "SDLPainter::clear_clip_rect(): SDL_RenderSetClipRect() failed: " << SDL_GetError() << std::endl;
  }
}

void
SDLPainter::get_pixel(const GetPixelRequest& request) const
{
  const Rect& rect = m_renderer.get_rect();
  const Sizef logical_size = m_renderer.get_logical_size();

  SDL_Rect srcrect;
  srcrect.x = rect.left + static_cast<int>(request.pos.x * static_cast<float>(rect.get_width()) / logical_size.width);
  srcrect.y = rect.top + static_cast<int>(request.pos.y * static_cast<float>(rect.get_height()) / logical_size.height);
  srcrect.w = 1;
  srcrect.h = 1;

  Uint8 pixel[4];
  int ret = SDL_RenderReadPixels(m_sdl_renderer, &srcrect,
                                 SDL_PIXELFORMAT_RGB888,
                                 pixel,
                                 1);
  if (ret != 0)
  {
    log_warning << "failed to read pixels: " << SDL_GetError() << std::endl;
  }

  *(request.color_ptr) = Color::from_rgb888(pixel[2], pixel[1], pixel[0]);
}

/* EOF */
