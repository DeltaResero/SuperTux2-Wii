//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#ifndef HEADER_SUPERTUX_OBJECT_BORDERED_HPP
#define HEADER_SUPERTUX_OBJECT_BORDERED_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

#include "math/rect.hpp"
#include "math/rectf.hpp"
#include "math/size.hpp"
#include "math/sizef.hpp"
#include "math/vector.hpp"
#include "video/surface.hpp"
#include "video/surface_ptr.hpp"

class Block;
class GameObjectManager;
class Tile;
class TileMap;
struct SDL_Surface;

/** Tiles drawn from copies bordered by what lies around them, so the pixel across a joint blends both sides */
namespace bordered {

/** Bends what's behind it where the renderer draws that, as OpenGL 3.3 does for a shine that isn't a mirror: drawn plain,
    but it still borders copies */
bool bends(const Surface* surface);

/** The RGBA pixels a picture is drawn from, or null where it can't be bordered */
const SDL_Surface* pixels_of(const Surface* surface);

/** Sides of a picture where nothing beside it draws the joint, so it draws them itself */
struct Open
{
  bool left;
  bool up;
  bool up_left;
  bool up_right;
};

/** Where to draw a picture at pos from its copy at origin, texels_x by texels_y inside a ring of one, half a texel along */
void place(const Vector& pos, const Sizef& size, const Vector& origin, int texels_x, int texels_y, const Open& open,
           std::vector<Rectf>& srcrects, std::vector<Rectf>& dstrects);

/** The tile layers alive, so the copies go with the last of them */
void hold();
void release();

/** Once a frame before anything draws, with the part of the level in view; the layers on the grid get their copies ready */
void begin_frame(GameObjectManager& objects, const void* scene, const Rectf& view);

/** Every layer on the grid has its copies this frame, so they're drawn from them; else all are drawn tile by tile */
bool grid_ready();

/** The tile layers drawn over map on the same grid */
std::vector<const TileMap*> layers_above(const GameObjectManager& objects, const TileMap& map);

/** A tile layer on the grid is fading, so all are drawn tile by tile this frame */
bool layers_fading(const GameObjectManager& objects);

/** Blocks resting on the grid near a part of the level, gathered once a frame */
void gather_blocks(const GameObjectManager& objects, const Rectf& near);
const Block* block_at(int x, int y);

/** The tile of a solid layer at cell x, y as it shows now, else a resting block's picture */
const Surface* grid_surface_at(const GameObjectManager& objects, int x, int y);

/** Another layer on the grid that's drawn has a tile at map's cell x, y, so the picture carries on there */
bool tile_beside(const GameObjectManager& objects, const TileMap& map, int x, int y);

/** The texture holding the copy of a block's picture from the 3 x 3 pictures round it, with the copy's place and size, or null */
SurfacePtr copy_of(const std::array<const Surface*, 9>& cells, Vector& origin, Size& texels);

/** A tile layer drawn from copies of its tiles, kept in a few textures shared by every layer */
class Layer final
{
public:
  /** The layers over one that hide what's under them, with their revisions */
  using Cover = std::vector<std::pair<const TileMap*, int>>;

  struct Draw
  {
    SurfacePtr page;
    std::vector<Rectf> srcrects;
    std::vector<Rectf> dstrects;
    // Shiny tiles come in draws of their own, with where each one shines
    std::vector<Shine> shines;
  };

public:
  Layer();

  /** False when the layer is to be drawn tile by tile this frame; tiles no copy can hold go in loose */
  bool draw(const TileMap& map, const Rect& cells, const std::vector<const TileMap*>& above,
            std::vector<Draw>& draws, std::vector<int>& loose);

  /** draw() for a layer on the grid, made before any layer draws and kept for take() */
  bool ready(const TileMap& map, const Rect& cells, const std::vector<const TileMap*>& above);

  /** What ready() made this frame, else false */
  bool take(std::vector<Draw>& draws, std::vector<int>& loose);

  /** Builds copies ahead, as time allows, for what will cover the layer once the fades are over */
  void prepare(const TileMap& map, const Rect& cells, const std::vector<const TileMap*>& above);

private:
  struct Cell
  {
    // The cell's index in the map, or -1
    int index;
    // Its copy, or -1 when the tile is drawn on its own
    int slot;
    uint32_t stamp;
    // Its neighbourhood in m_moving when a tile round it animates, else -1
    int moving;
    Open open;
  };

  /** A copy for one set of animation frames, its slot as a cell's */
  struct Shown
  {
    std::array<uint8_t, 9> frames;
    int slot;
    uint32_t stamp;
  };

  /** Tiles round a cell with one that animates, shared by every cell with the same and brought up to date once a frame */
  struct Moving
  {
    std::array<const Tile*, 9> tiles;
    std::vector<const Surface*> over;
    // The copy of each set of frames it has shown, kept while it's drawn so a long animation doesn't make them again
    std::vector<Shown> shown;
    // The one showing now, as of the frame it was checked
    size_t now;
    int checked;
  };

  void refresh(const TileMap& map, const Rect& cells, const Cover& cover);
  Cell& cell_at(int x, int y);
  bool current(const TileMap& map, int x, int y, const Cell& cell) const;
  /** False when there's no room left for its copy */
  bool resolve(const TileMap& map, int x, int y, const Cover& cover, Cell& cell);
  /** False when it hasn't shown its tiles' frames now yet */
  bool find_shown(Moving& moving) const;
  /** False when there's no room left for its copy */
  bool add_shown(Moving& moving) const;
  void prefetch(const TileMap& map, const Rect& cells, const Cover& cover);

private:
  // Cells near the screen, each at its x and y wrapped round the size
  std::vector<Cell> m_cells;
  std::vector<Moving> m_moving;
  std::map<std::pair<std::array<const Tile*, 9>, std::vector<const Surface*>>, int> m_moving_index;
  Size m_size;
  int m_revision;
  int m_generation;
  Cover m_cover;
  // What ready() made, and in which frame
  std::vector<Draw> m_draws;
  std::vector<int> m_loose;
  int m_ready_frame;

private:
  Layer(const Layer&) = delete;
  Layer& operator=(const Layer&) = delete;
};

} // namespace bordered

#endif

/* EOF */
