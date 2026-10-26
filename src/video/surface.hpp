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

#ifndef HEADER_SUPERTUX_VIDEO_SURFACE_HPP
#define HEADER_SUPERTUX_VIDEO_SURFACE_HPP

#include <algorithm>
#include <string>
#include <optional>
#include <vector>

#include "math/rect.hpp"
#include "math/rectf.hpp"
#include "math/vector.hpp"
#include "video/flip.hpp"
#include "video/surface_ptr.hpp"
#include "video/texture_ptr.hpp"

class ReaderMapping;
class SurfaceData;

/** How much of a mirror image of what's above shows across a shiny picture, at corners every STEP_X pixels across
    and STEP_Y down from its top, down to height where it ends */
struct Reflection
{
  static constexpr int STEP_X = 8;
  static constexpr int STEP_Y = 4;

  int width = 0;
  int height = 0;
  /** Row by row from the top, columns() each */
  std::vector<float> shown = {};

  int columns() const { return (width + STEP_X - 1) / STEP_X + 1; }
  int rows() const { return (height + STEP_Y - 1) / STEP_Y + 1; }
  float x(int column) const { return static_cast<float>(std::min(column * STEP_X, width)); }
  float y(int row) const { return static_cast<float>(std::min(row * STEP_Y, height)); }
  float at(int column, int row) const { return shown[static_cast<size_t>(row * columns() + column)]; }
};

/** A part of a shiny picture, in its own pixels, and where it's drawn, with the reflection it shows there or null for none */
struct Shine
{
  const Reflection* reflection;
  Rectf srcrect;
  Rectf dstrect;
};

/** A rectangular image.  The class basically holds a reference to a
    texture with additional UV coordinates that specify a rectangular
    area on this texture */
class Surface final
{
public:
  static SurfacePtr from_texture(const TexturePtr& texture);
  static SurfacePtr from_file(const std::string& filename, const std::optional<Rect>& rect = std::nullopt);
  static SurfacePtr from_reader(const ReaderMapping& mapping, const std::optional<Rect>& rect = std::nullopt);

private:
  Surface(const TexturePtr& diffuse_texture, const TexturePtr& displacement_texture, Flip flip,
          std::optional<Reflection> reflection = std::nullopt, SurfacePtr plain = SurfacePtr());
  Surface(const TexturePtr& diffuse_texture, const TexturePtr& displacement_texture, const Rect& region, Flip flip,
          std::optional<Reflection> reflection = std::nullopt, SurfacePtr plain = SurfacePtr());

public:
  ~Surface();

  SurfacePtr region(const Rect& rect) const;
  SurfacePtr clone(Flip flip = NO_FLIP) const;

  const TexturePtr& get_texture() const;
  const TexturePtr& get_displacement_texture() const;
  Rect get_region() const { return m_region; }
  int get_width() const;
  int get_height() const;
  Flip get_flip() const { return m_flip; }

  /** The mirror image of what's above it that a shiny picture shows, or null for a picture that isn't a mirror */
  const Reflection* get_reflection() const { return m_reflection ? &*m_reflection : nullptr; }

  /** A mirror's picture without its shine, for where something in front lies on its top, else null */
  const SurfacePtr& get_plain() const { return m_plain; }

private:
  const TexturePtr m_diffuse_texture;
  const TexturePtr m_displacement_texture;
  const Rect m_region;
  const Flip m_flip;
  const std::optional<Reflection> m_reflection;
  const SurfacePtr m_plain;

private:
  Surface& operator=(const Surface&) = delete;
};

#endif

/* EOF */
