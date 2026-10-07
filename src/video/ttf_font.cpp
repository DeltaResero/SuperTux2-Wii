//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
//  Copyright (C) 2018 Tobias Markus <tobbi.bugs@googlemail.com>
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

#include "video/ttf_font.hpp"

#include <iostream>
#include <numeric>
#include <sstream>

#include "util/line_iterator.hpp"
#include "io/sdl_file.hpp"
#include "video/canvas.hpp"
#include "video/drawing_context.hpp"
#include "video/paint_style.hpp"
#include "video/surface.hpp"
#include "video/ttf_surface_manager.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

TTFFont::TTFFont(const std::string& filename, int font_size, float line_spacing, int shadow_size, int border) :
  m_font(),
  m_filename(filename),
  m_font_size(font_size),
  m_line_spacing(line_spacing),
  m_shadow_size(shadow_size),
  m_border(border),
  m_sized_fonts()
{
  m_font = TTF_OpenFontRW(get_SDLRWops(m_filename), 1, font_size);
  if (!m_font)
  {
    std::ostringstream msg;
    msg << "Couldn't load TTFFont: " << m_filename << ": " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
}

TTFFont::~TTFFont()
{
  for (const auto& sized_font : m_sized_fonts)
  {
    TTF_CloseFont(sized_font.second);
  }
  TTF_CloseFont(m_font);
}

TTF_Font*
TTFFont::get_ttf_font(int pixel_size, unsigned int horizontal_dpi) const
{
  if (pixel_size == m_font_size && horizontal_dpi == 72)
  {
    return m_font;
  }

  const auto key = std::make_pair(pixel_size, horizontal_dpi);
  auto it = m_sized_fonts.find(key);
  if (it != m_sized_fonts.end())
  {
    return it->second;
  }

#if SDL_TTF_VERSION_ATLEAST(2, 0, 18)
  // Letters drawn narrower or wider than tall come from a horizontal resolution apart from the vertical one
  TTF_Font* font = (horizontal_dpi == 72) ?
    TTF_OpenFontRW(get_SDLRWops(m_filename), 1, pixel_size) :
    TTF_OpenFontDPIRW(get_SDLRWops(m_filename), 1, pixel_size, horizontal_dpi, 72);
#else
  TTF_Font* font = TTF_OpenFontRW(get_SDLRWops(m_filename), 1, pixel_size);
#endif
  if (!font)
  {
    std::cerr << "TTFFont::get_ttf_font(): " << TTF_GetError() << std::endl;
    return m_font;
  }
  return m_sized_fonts[key] = font;
}

float
TTFFont::get_text_width(const std::string& text) const
{
  if (text.empty())
    return 0.0f;

  float max_width = 0.0f;

  LineIterator iter(text);
  while (iter.next())
  {
    const std::string& line = iter.get();

    // Since get_cached_surface_width() takes a surface from the cache
    // instead of generating it from scratch,
    // it should be faster than doing a whole layout.
    float line_width = TTFSurfaceManager::current()->get_cached_surface_width(*this, line);
    if (line_width < 0.0f) {
      // Not in cache
      int w = 0;
      int h = 0;
      int ret = TTF_SizeUTF8(m_font, line.c_str(), &w, &h);
      if (ret < 0) {
        std::cerr << "TTFFont::get_text_width(): " << TTF_GetError() << std::endl;
      }
      line_width = static_cast<float>(w);
    }
    max_width = std::max(max_width, line_width);
  }

  return max_width;
}

float
TTFFont::get_text_height(const std::string& text) const
{
  if (text.empty())
    return 0.0f;

  // since UTF8 multibyte characters are decoded with values
  // outside the ASCII range there is no risk of overlapping and
  // thus we don't need to decode the utf-8 string
  return std::accumulate(text.begin(), text.end(), get_height(), [this] (float accumulator, const char c) {
    return accumulator += (c == '\n' ? get_height() : 0.0f);
  });
}

void
TTFFont::draw_text(Canvas& canvas, const std::string& text,
                   const Vector& pos, FontAlignment alignment, int layer, const Color& color)

{
  // Rendered at the size it covers on screen, so it's never stretched
  const Vector pixel_scale = VideoSystem::current()->get_viewport().get_scale() * canvas.get_context().transform().scale;

  float last_y = pos.y - (static_cast<float>(TTF_FontHeight(m_font)) - get_height()) / 2.0f;

  LineIterator iter(text);
  while (iter.next())
  {
    const std::string& line = iter.get();

    if (!line.empty())
    {
      TTFSurfacePtr ttf_surface = TTFSurfaceManager::current()->create_surface(*this, line, pixel_scale);

      Vector new_pos(pos.x, last_y);

      if (alignment == ALIGN_CENTER)
      {
        new_pos.x -= ttf_surface->get_size().width / 2.0f;
      }
      else if (alignment == ALIGN_RIGHT)
      {
        new_pos.x -= ttf_surface->get_size().width;
      }

      // draw text
      if (ttf_surface->get_surface())
      {
        canvas.draw_surface_scaled(ttf_surface->get_surface(), Rectf(glm::floor(new_pos), ttf_surface->get_size()),
                                   layer, PaintStyle().set_color(color));
      }
    }

    last_y += get_height();
  }
}

std::string
TTFFont::wrap_to_width(const std::string& text, float width, std::string* overflow)
{
  std::string s = text;

  // if text is already smaller, return full text
  if (get_text_width(s) <= width) {
    if (overflow) *overflow = "";
    return s;
  }

  // if we can find a whitespace character to break at, return text up to this character
  for (int i = static_cast<int>(s.length()) - 1; i >= 0; i--) {
    std::string s2 = s.substr(0,i);
    if (s[i] != ' ') continue;
    if (get_text_width(s2) <= width) {
      if (overflow) *overflow = s.substr(i+1);
      return s.substr(0, i);
    }
  }

  // hard-wrap at width, taking care of multibyte characters
  unsigned int char_bytes = 1;
  for (int i = 0; i < static_cast<int>(s.length()); i += char_bytes) {

    // calculate the number of bytes in the character
    char_bytes = 1;
    auto iter = s.begin() + i + 1; // iter points to next byte
    while ( iter != s.end() && (*iter & 128) && !(*iter & 64) ) {
      // this is a "continuation" byte in the form 10xxxxxx
      ++iter;
      ++char_bytes;
    }

    // check whether text now goes over allowed width, and if so
    // return everything up to the character and put the rest in the overflow
    std::string s2 = s.substr(0,i+char_bytes);
    if (get_text_width(s2) > width) {
      if (i == 0) i += char_bytes; // edge case when even one char is too wide
      if (overflow) *overflow = s.substr(i);
      return s.substr(0, i);
    }
  }

  // should in theory never reach here
  if (overflow) *overflow = "";
  return s;
}

/* EOF */
