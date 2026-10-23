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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_PAINTER_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_PAINTER_HPP

#include "video/painter.hpp"

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

#include "math/vector.hpp"

class Rectf;
class Renderer;
class SDLScreenRenderer;
class SDLTexture;
class SDLVideoSystem;
struct DrawingRequest;
struct SDL_FRect;
struct SDL_Renderer;
struct SDL_Texture;

class SDLPainter final : public Painter
{
public:
  SDLPainter(SDLVideoSystem& video_system, Renderer& renderer, SDL_Renderer* sdl_renderer, float scale);
  ~SDLPainter() override;

  /** Scales every coordinate to whole pixels here, with SDL left at a scale of 1 */
  void set_pixel_scale(const Vector& scale);

  /** Lets go of tinted copies that haven't been drawn for a while */
  void start_frame();

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
  SDL_FRect to_target(const Rectf& rect, bool between_pixels) const;
  SDL_FRect to_sdl_frect(const SDL_Rect& rect) const;
  /** False for a rectangle that's empty on whole pixels, as real SDL2's software renderer still fills a row */
  bool shows(const SDL_FRect& rect) const;
  /** To a whole pixel, halves rounded down in the software renderer, so a picture crossing the top or left edge keeps its size */
  float edge(float pixels) const;
  /** Fills rect, rounded by radius, in whole screen pixels */
  void fill_whole_pixels(const Rectf& rect, float radius) const;
  /** Fills rect in one colour, unless it comes out empty on whole pixels */
  void fill_rect(const SDL_Rect& rect, SDL_BlendMode blend, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;

  /** A rectangle's edges on whole screen pixels, with the scale and origin that turn them back into SDL's units */
  struct ScreenEdges
  {
    float left;
    float top;
    float right;
    float bottom;
    Vector scale;
    Vector origin;
  };
  ScreenEdges screen_edges(const Rectf& rect) const;
  /** A copy of texture with the colour already applied, made the first time it's needed, or null when it can't have one */
  SDL_Texture* tinted(const SDLTexture& texture, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

private:
  struct Tinted
  {
    uint32_t color;
    SDL_Texture* texture;
    uint32_t drawn;
  };

private:
  SDLVideoSystem& m_video_system;
  Renderer& m_renderer;
  SDL_Renderer* m_sdl_renderer;

  // Applied to every coordinate before it reaches SDL
  Vector m_scale;
  bool m_whole_pixels;

  std::optional<SDL_Rect> m_cliprect;

  // By the serial of the texture they were made from
  std::unordered_map<uint32_t, std::vector<Tinted>> m_tinted;
  uint32_t m_frame;

private:
  SDLPainter(const SDLPainter&) = delete;
  SDLPainter& operator=(const SDLPainter&) = delete;
};

#endif

/* EOF */
