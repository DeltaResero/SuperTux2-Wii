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

#include "object/bordered.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <unordered_map>

#include <SDL.h>

#include "object/block.hpp"
#include "object/tilemap.hpp"
#include "supertux/game_object_manager.hpp"
#include "supertux/tile.hpp"
#include "video/sampler.hpp"
#include "video/sdl_surface.hpp"
#include "video/surface.hpp"
#include "video/texture.hpp"
#include "video/texture_manager.hpp"
#include "video/video_system.hpp"

namespace bordered {

namespace {

/** What a copy is made from: the 3 x 3 pictures round a tile, then 3 x 3 more for each layer over it whose edges reach it */
struct Key
{
  // Tile and sprite pictures last as long as the game, so an address stays one picture's
  std::array<const Surface*, 9> cells;
  std::vector<const Surface*> over;

  bool operator==(const Key& other) const { return cells == other.cells && over == other.over; }
};

struct KeyHash
{
  size_t operator()(const Key& key) const
  {
    size_t hash = 0;
    for (const Surface* surface : key.cells)
    {
      hash = hash * 1000003u ^ std::hash<const Surface*>()(surface);
    }
    for (const Surface* surface : key.over)
    {
      hash = hash * 1000003u ^ std::hash<const Surface*>()(surface);
    }
    return hash;
  }
};

// The copies are kept in pages of this many texels square
constexpr int PAGE = 512;
constexpr size_t MAX_PAGES = 32;
// A copy undrawn this many ms gives up its slot
constexpr Uint32 KEEP = 2000;

struct Page
{
  SurfacePtr picture;
  std::vector<int> free;
};

struct Slot
{
  Uint32 last;
  uint32_t stamp;
  // The copy in it, or null when it's free
  const Key* key;
};

std::unordered_map<Key, int, KeyHash> s_copies;
std::vector<Page> s_pages;
std::vector<Slot> s_slots;
// A tile's texels, set by the first copy, and how many copies fit a page
Size s_texels(0, 0);
int s_per_row = 0;
int s_per_page = 0;
uint32_t s_stamps = 0;
Uint32 s_now = 0;
Uint32 s_swept = 0;
int s_frame = 0;
int s_generation = -1;
int s_holders = 0;
// Time left this frame, in ms, for copies on screen, for after a fade and for just off screen
double s_urgent = 0.0;
double s_prepare = 0.0;
double s_prefetch = 0.0;
// The first frame of a scene builds all it shows, as part of loading
const void* s_scene = nullptr;
int s_scene_frame = -1;
bool s_scene_start = false;
// What a picture is to what's under it, by its alpha
enum Kind : uint8_t { CLEAR, OPAQUE, MIXED };
std::unordered_map<const Surface*, uint8_t> s_kinds;
// The tile layers on the grid in drawing order, taken once a frame
std::vector<const TileMap*> s_layers;
int s_layers_frame = -1;
bool s_layers_fading = false;
std::unordered_map<int64_t, const Block*> s_blocks;

template<typename Work>
void
timed(double& budget, const Work& work)
{
  const Uint64 start = SDL_GetPerformanceCounter();
  work();
  budget -= static_cast<double>(SDL_GetPerformanceCounter() - start) * 1000.0 /
            static_cast<double>(SDL_GetPerformanceFrequency());
}

const Uint8*
texel(const SDL_Surface& surface, int x, int y)
{
  return static_cast<const Uint8*>(surface.pixels) + y * surface.pitch + x * 4;
}

int64_t
cell_key(int x, int y)
{
  return (static_cast<int64_t>(x) << 32) ^ static_cast<int64_t>(static_cast<uint32_t>(y));
}

uint32_t
id_at(const TileMap& map, int x, int y)
{
  return (x < 0 || y < 0 || x >= map.get_width() || y >= map.get_height()) ? 0 : map.get_tile_id(x, y);
}

/** A picture drawn whole and unflipped from its texture, else null */
const Texture*
whole(const Surface* surface)
{
  if (!surface || surface->get_flip() != NO_FLIP || !surface->get_texture())
  {
    return nullptr;
  }
  const Texture& texture = *surface->get_texture();
  const Rect region = surface->get_region();
  if (region.left != 0 || region.top != 0 ||
      region.get_width() != texture.get_image_width() || region.get_height() != texture.get_image_height())
  {
    return nullptr;
  }
  return &texture;
}

/** A tile-sized picture whose pixels a copy can take, else null; its pixels are only made once a copy needs them */
const Surface*
fitting(const Surface* surface)
{
  const Texture* texture = whole(surface);
  if (!texture || surface->get_width() != 32 || surface->get_height() != 32)
  {
    return nullptr;
  }
  const Size texels = TextureManager::current()->get_pixel_size(*texture);
  if (texels.width == 0) return nullptr;
  return (s_texels.width == 0 || texels == s_texels) ? surface : nullptr;
}

/** SDLSurface::fill_clear_pixels on grid, only inside regions */
void
fill_clear(std::vector<uint32_t>& grid, int gw, int gh, const std::vector<Rect>& regions)
{
  const auto bytes = [&grid](size_t i) { return reinterpret_cast<Uint8*>(&grid[i]); };
  std::vector<Uint8> known(grid.size(), 0);
  for (const Rect& region : regions)
  {
    for (int y = region.top; y < region.bottom; ++y)
    {
      for (int x = region.left; x < region.right; ++x)
      {
        const size_t i = static_cast<size_t>(y * gw + x);
        known[i] = bytes(i)[3] > 0;
      }
    }
  }
  for (int ring = 0; ring < 2; ++ring)
  {
    std::vector<Uint8> next = known;
    for (const Rect& region : regions)
    {
      for (int y = region.top; y < region.bottom; ++y)
      {
        for (int x = region.left; x < region.right; ++x)
        {
          const size_t i = static_cast<size_t>(y * gw + x);
          if (known[i]) continue;
          int red = 0, green = 0, blue = 0, count = 0;
          for (int ny = std::max(y - 1, 0); ny <= std::min(y + 1, gh - 1); ++ny)
          {
            for (int nx = std::max(x - 1, 0); nx <= std::min(x + 1, gw - 1); ++nx)
            {
              const size_t j = static_cast<size_t>(ny * gw + nx);
              if (!known[j]) continue;
              red += bytes(j)[0];
              green += bytes(j)[1];
              blue += bytes(j)[2];
              ++count;
            }
          }
          if (count == 0) continue;
          Uint8* out = bytes(i);
          out[0] = static_cast<Uint8>(red / count);
          out[1] = static_cast<Uint8>(green / count);
          out[2] = static_cast<Uint8>(blue / count);
          next[i] = 1;
        }
      }
    }
    known.swap(next);
  }
}

uint8_t
kind_of(const Surface* surface)
{
  const SDL_Surface* pixels = pixels_of(surface);
  if (!pixels) return CLEAR;
  const auto it = s_kinds.find(surface);
  if (it != s_kinds.end()) return it->second;
  bool opaque = true;
  bool clear = true;
  for (int y = 0; y < pixels->h; ++y)
  {
    for (int x = 0; x < pixels->w; ++x)
    {
      const Uint8 alpha = texel(*pixels, x, y)[3];
      opaque = opaque && alpha >= 128;
      clear = clear && alpha < 32;
    }
  }
  const uint8_t kind = clear ? CLEAR : (opaque ? OPAQUE : MIXED);
  s_kinds[surface] = kind;
  return kind;
}

/** Two texels that tell apart at a glance, so showing one in the other's place would show */
bool
apart(uint32_t a, uint32_t b)
{
  const Uint8* x = reinterpret_cast<const Uint8*>(&a);
  const Uint8* y = reinterpret_cast<const Uint8*>(&b);
  for (int c = 0; c < 4; ++c)
  {
    if (std::abs(static_cast<int>(x[c]) - static_cast<int>(y[c])) > 40) return true;
  }
  return false;
}

/** The copy's texels, (w + 2) by (h + 2), as one picture of the 3 x 3 tiles and the layers over them has them;
    with changed, only whether the layers over them change any texel it takes */
std::vector<uint32_t>
compute(const Key& key, int w, int h, bool* changed = nullptr)
{
  // As far round the copy as the junction rule and the fill reach, so its ring comes out as the copies beside it have it
  const Rect grid(-4, -4, w + 4, h + 4);
  const int gw = grid.get_width();
  const int gh = grid.get_height();
  const auto at = [gw](int i, int j) { return static_cast<size_t>((j + 4) * gw + (i + 4)); };
  const auto inside = [&grid](int i, int j) { return i >= grid.left && j >= grid.top && i < grid.right && j < grid.bottom; };
  const auto cell_of = [](int cx, int cy) { return static_cast<size_t>((cy + 1) * 3 + (cx + 1)); };
  const auto span = [w, h, &grid](int cx, int cy) {
    return Rect(std::max(cx * w, grid.left), std::max(cy * h, grid.top),
                std::min(cx * w + w, grid.right), std::min(cy * h + h, grid.bottom));
  };
  const auto each = [&](const SDL_Surface& pixels, int cx, int cy, const auto& put) {
    const Rect texels = span(cx, cy);
    for (int j = texels.top; j < texels.bottom; ++j)
    {
      put(at(texels.left, j), texel(pixels, texels.left - cx * w, j - cy * h), texels.right - texels.left);
    }
  };
  const auto fits = [w, h](const Surface* surface) -> const SDL_Surface* {
    const SDL_Surface* pixels = pixels_of(surface);
    return (pixels && pixels->w == w && pixels->h == h) ? pixels : nullptr;
  };

  std::vector<uint32_t> lower(static_cast<size_t>(gw * gh), 0);
  for (int cy = -1; cy <= 1; ++cy)
  {
    for (int cx = -1; cx <= 1; ++cx)
    {
      const SDL_Surface* pixels = fits(key.cells[cell_of(cx, cy)]);
      if (!pixels) continue;
      each(*pixels, cx, cy, [&lower](size_t to, const Uint8* from, int count) {
        std::memcpy(&lower[to], from, static_cast<size_t>(count) * 4);
      });
    }
  }

  // Each layer over this one, cell by cell, by its first frame, as only its shape matters
  const size_t layers = std::min<size_t>(key.over.size() / 9, 32);
  std::vector<std::array<uint8_t, 9>> kinds(layers);
  std::array<bool, 9> edged{};
  bool any_edge = false;
  for (size_t k = 0; k < layers; ++k)
  {
    for (size_t c = 0; c < 9; ++c)
    {
      kinds[k][c] = fits(key.over[k * 9 + c]) ? kind_of(key.over[k * 9 + c]) : uint8_t(CLEAR);
    }
    for (int cy = -1; cy <= 1; ++cy)
    {
      for (int cx = -1; cx <= 1; ++cx)
      {
        const uint8_t kind = kinds[k][cell_of(cx, cy)];
        bool edge = kind == MIXED;
        if (kind == OPAQUE)
        {
          for (const auto& step : { std::pair<int, int>{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } })
          {
            const int nx = cx + step.first;
            const int ny = cy + step.second;
            edge = edge || nx < -1 || ny < -1 || nx > 1 || ny > 1 || kinds[k][cell_of(nx, ny)] != OPAQUE;
          }
        }
        if (edge)
        {
          edged[cell_of(cx, cy)] = true;
          any_edge = true;
        }
      }
    }
  }

  std::vector<uint32_t> out = lower;
  if (any_edge)
  {
    // A bit per layer over this one, as each layer's edge is judged on its own
    std::vector<uint32_t> opaque(static_cast<size_t>(gw * gh), 0);
    std::vector<uint32_t> solid(static_cast<size_t>(gw * gh), 0);
    const auto near_edge = [&edged, &cell_of](int cx, int cy) {
      for (const auto& step : { std::pair<int, int>{ 0, 0 }, { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } })
      {
        const int nx = cx + step.first;
        const int ny = cy + step.second;
        if (nx >= -1 && ny >= -1 && nx <= 1 && ny <= 1 && edged[cell_of(nx, ny)]) return true;
      }
      return false;
    };
    for (size_t k = 0; k < layers; ++k)
    {
      const uint32_t bit = uint32_t(1) << k;
      for (int cy = -1; cy <= 1; ++cy)
      {
        for (int cx = -1; cx <= 1; ++cx)
        {
          if (kinds[k][cell_of(cx, cy)] == CLEAR || !near_edge(cx, cy)) continue;
          each(*fits(key.over[k * 9 + cell_of(cx, cy)]), cx, cy, [&opaque, &solid, bit](size_t to, const Uint8* from, int count) {
            for (int n = 0; n < count; ++n)
            {
              const Uint8 alpha = from[n * 4 + 3];
              if (alpha >= 128) opaque[to + static_cast<size_t>(n)] |= bit;
              if (alpha >= 32) solid[to + static_cast<size_t>(n)] |= bit;
            }
          });
        }
      }
    }
    // Under a covering layer's edge, this one shows what lies just outside it, so the edge blends into that and not into what the layer hides
    for (int cy = -1; cy <= 1; ++cy)
    {
      for (int cx = -1; cx <= 1; ++cx)
      {
        if (!edged[cell_of(cx, cy)]) continue;
        const Rect texels = span(cx, cy);
        for (int j = std::max(texels.top, -3); j < std::min(texels.bottom, h + 3); ++j)
        {
          for (int i = std::max(texels.left, -3); i < std::min(texels.right, w + 3); ++i)
          {
            const size_t p = at(i, j);
            if (!opaque[p]) continue;
            for (const auto& step : { std::pair<int, int>{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } })
            {
              if (!inside(i + step.first, j + step.second)) continue;
              const size_t q = at(i + step.first, j + step.second);
              if (opaque[p] & ~solid[q])
              {
                if (apart(lower[p], lower[q]))
                {
                  if (changed)
                  {
                    *changed = true;
                    return {};
                  }
                  out[p] = lower[q];
                }
                break;
              }
            }
          }
        }
      }
    }
  }

  if (changed) return {};
  fill_clear(out, gw, gh, { Rect(0, 0, gw, gh) });

  std::vector<uint32_t> copy(static_cast<size_t>((w + 2) * (h + 2)));
  for (int j = -1; j <= h; ++j)
  {
    std::memcpy(&copy[static_cast<size_t>((j + 1) * (w + 2))], &out[at(-1, j)], static_cast<size_t>(w + 2) * 4);
  }
  return copy;
}

void
free_slot(int index)
{
  Slot& slot = s_slots[static_cast<size_t>(index)];
  if (!slot.key) return;
  const auto it = s_copies.find(*slot.key);
  slot.key = nullptr;
  if (it != s_copies.end()) s_copies.erase(it);
  slot.stamp = ++s_stamps;
  s_pages[static_cast<size_t>(index / s_per_page)].free.push_back(index);
}

/** Frees the slots of copies undrawn for longer than keep ms */
void
sweep(Uint32 keep)
{
  for (size_t i = 0; i < s_slots.size(); ++i)
  {
    if (s_slots[i].key && s_now - s_slots[i].last > keep) free_slot(static_cast<int>(i));
  }
}

/** A free slot, or -1 when every page is full */
int
take_slot()
{
  for (int attempt = 0; attempt < 2; ++attempt)
  {
    for (Page& page : s_pages)
    {
      if (page.picture && !page.free.empty())
      {
        const int index = page.free.back();
        page.free.pop_back();
        return index;
      }
    }
    if (attempt == 0) sweep(KEEP);
  }

  // A new page, in place of one let go of if there is one
  size_t number = 0;
  while (number < s_pages.size() && s_pages[number].picture) ++number;
  if (number == s_pages.size())
  {
    if (s_pages.size() >= MAX_PAGES) return -1;
    s_pages.emplace_back();
    for (int i = s_per_page - 1; i >= 0; --i)
    {
      s_pages.back().free.push_back(static_cast<int>(number) * s_per_page + i);
    }
    s_slots.resize(s_slots.size() + static_cast<size_t>(s_per_page), Slot{ 0, 0, nullptr });
  }
  Page& page = s_pages[number];
  SDLSurfacePtr blank = SDLSurface::create_rgba(PAGE, PAGE);
  page.picture = Surface::from_texture(VideoSystem::current()->new_texture(*blank, Sampler(), Size(PAGE, PAGE)));
  const int index = page.free.back();
  page.free.pop_back();
  return index;
}

Vector
origin_of(int index)
{
  const int i = index % s_per_page;
  return Vector(static_cast<float>((i % s_per_row) * (s_texels.width + 2)),
                static_cast<float>((i / s_per_row) * (s_texels.height + 2)));
}

/** Makes key's copy in the slot at index */
void
put(int index, const Key& key)
{
  const int w = s_texels.width;
  const int h = s_texels.height;
  const std::vector<uint32_t> texels = compute(key, w, h);
  SDLSurfacePtr copy = SDLSurface::create_rgba(w + 2, h + 2);
  for (int y = 0; y < h + 2; ++y)
  {
    std::memcpy(static_cast<Uint8*>(copy->pixels) + y * copy->pitch, &texels[static_cast<size_t>(y * (w + 2))],
                static_cast<size_t>(w + 2) * 4);
  }
  const Vector origin = origin_of(index);
  s_pages[static_cast<size_t>(index / s_per_page)].picture->get_texture()->update(
    *copy, static_cast<int>(origin.x), static_cast<int>(origin.y));
}

/** The first copy sets how big a tile's copy is */
void
size_copies(const Surface& center)
{
  if (s_texels.width != 0) return;
  s_texels = TextureManager::current()->get_pixel_size(*center.get_texture());
  s_per_row = PAGE / (s_texels.width + 2);
  s_per_page = s_per_row * (PAGE / (s_texels.height + 2));
}

/** The slot holding key's copy, built if need be, or -1 when there's no room */
int
find_or_build(Key&& key)
{
  const auto it = s_copies.find(key);
  if (it != s_copies.end()) return it->second;

  size_copies(*key.cells[4]);
  const int index = take_slot();
  if (index < 0) return -1;
  put(index, key);

  Slot& slot = s_slots[static_cast<size_t>(index)];
  slot.key = &s_copies.emplace(std::move(key), index).first->first;
  slot.stamp = ++s_stamps;
  slot.last = s_now;
  return index;
}

/** Moves copies out of the page holding fewest while the others have room, for budget ms, so it can let go of its texture */
void
compact(double budget)
{
  size_t source = s_pages.size();
  size_t fewest = 0;
  size_t room = 0;
  for (size_t number = 0; number < s_pages.size(); ++number)
  {
    if (!s_pages[number].picture) continue;
    const size_t held = static_cast<size_t>(s_per_page) - s_pages[number].free.size();
    room += s_pages[number].free.size();
    if (source == s_pages.size() || held < fewest)
    {
      source = number;
      fewest = held;
    }
  }
  if (source == s_pages.size()) return;
  room -= s_pages[source].free.size();
  // With a quarter of a page left over, so new copies don't need it again straight away
  if (room < fewest + static_cast<size_t>(s_per_page / 4)) return;

  const int first = static_cast<int>(source) * s_per_page;
  for (int from = first; from < first + s_per_page && budget > 0.0; ++from)
  {
    Slot& origin = s_slots[static_cast<size_t>(from)];
    if (!origin.key) continue;
    int to = -1;
    for (size_t number = 0; number < s_pages.size() && to < 0; ++number)
    {
      Page& page = s_pages[number];
      if (number == source || !page.picture || page.free.empty()) continue;
      to = page.free.back();
      page.free.pop_back();
    }
    if (to < 0) break;
    timed(budget, [&] { put(to, *origin.key); });
    Slot& target = s_slots[static_cast<size_t>(to)];
    target.key = origin.key;
    target.last = origin.last;
    target.stamp = ++s_stamps;
    s_copies.find(*origin.key)->second = to;
    origin.key = nullptr;
    origin.stamp = ++s_stamps;
    s_pages[source].free.push_back(from);
  }
  if (static_cast<int>(s_pages[source].free.size()) == s_per_page) s_pages[source].picture.reset();
}

void
clear_all()
{
  s_copies.clear();
  s_pages.clear();
  s_slots.clear();
  s_kinds.clear();
  s_texels = Size(0, 0);
  s_per_row = 0;
  s_per_page = 0;
}

void
take_layers(const GameObjectManager& objects)
{
  if (s_layers_frame == s_frame) return;
  s_layers_frame = s_frame;
  s_layers.clear();
  s_layers_fading = false;
  for (const TileMap& layer : objects.get_objects_by_type<TileMap>())
  {
    if (layer.get_offset() == Vector(0.0f, 0.0f) && !layer.get_path() && layer.get_flip() == NO_FLIP &&
        layer.get_speed() == Vector(1.0f, 1.0f))
    {
      s_layers.push_back(&layer);
      s_layers_fading = s_layers_fading || layer.is_fading();
    }
  }
  std::stable_sort(s_layers.begin(), s_layers.end(),
                   [](const TileMap* a, const TileMap* b) { return a->get_layer() < b->get_layer(); });
}

/** The layers of above that hide what's under them now */
Layer::Cover
cover_of(const std::vector<const TileMap*>& above)
{
  Layer::Cover cover;
  for (const TileMap* layer : above)
  {
    if (layer->get_alpha() >= 1.0f && !layer->is_fading()) cover.emplace_back(layer, layer->get_revision());
  }
  return cover;
}

/** And once the fades are over */
Layer::Cover
cover_after(const std::vector<const TileMap*>& above)
{
  Layer::Cover cover;
  for (const TileMap* layer : above)
  {
    if (layer->get_target_alpha() >= 1.0f) cover.emplace_back(layer, layer->get_revision());
  }
  return cover;
}

} // namespace

const SDL_Surface*
pixels_of(const Surface* surface)
{
  const Texture* texture = whole(surface);
  if (!texture) return nullptr;
  const SDL_Surface* pixels = TextureManager::current()->get_pixels(*texture);
  return (pixels && pixels->format->format == SDL_PIXELFORMAT_RGBA32) ? pixels : nullptr;
}

void
place(const Vector& pos, const Sizef& size, const Vector& origin, int texels_x, int texels_y, const Open& open,
      std::vector<Rectf>& srcrects, std::vector<Rectf>& dstrects)
{
  // Half a texel in level units; the copy's own texels start 1 in from its edge
  const float w = static_cast<float>(texels_x);
  const float h = static_cast<float>(texels_y);
  const float half_x = size.width / w / 2.0f;
  const float half_y = size.height / h / 2.0f;
  const float left = pos.x;
  const float top = pos.y;
  const float right = pos.x + size.width;
  const float bottom = pos.y + size.height;
  const float u = origin.x;
  const float v = origin.y;
  // Its open sides stretch the one quad where no corner above belongs to a picture beside it
  const bool up = open.up && open.up_right && (!open.left || open.up_left);
  if (up || (open.left && !open.up))
  {
    const bool left_too = open.left;
    srcrects.emplace_back(u + (left_too ? 0.5f : 1.5f), v + (up ? 0.5f : 1.5f), u + w + 1.5f, v + h + 1.5f);
    dstrects.emplace_back(left_too ? left - half_x : left + half_x, up ? top - half_y : top + half_y,
                          right + half_x, bottom + half_y);
    return;
  }
  srcrects.emplace_back(u + 1.5f, v + 1.5f, u + w + 1.5f, v + h + 1.5f);
  dstrects.emplace_back(left + half_x, top + half_y, right + half_x, bottom + half_y);
  if (open.left)
  {
    srcrects.emplace_back(u + 0.5f, v + 1.5f, u + 1.5f, v + h + 1.5f);
    dstrects.emplace_back(left - half_x, top + half_y, left + half_x, bottom + half_y);
  }
  if (open.up)
  {
    // A corner above goes to the picture beside this one where that one draws there
    const bool up_left = open.left && open.up_left;
    srcrects.emplace_back(u + (up_left ? 0.5f : 1.5f), v + 0.5f, u + (open.up_right ? w + 1.5f : w + 0.5f), v + 1.5f);
    dstrects.emplace_back(up_left ? left - half_x : left + half_x, top - half_y,
                          open.up_right ? right + half_x : right - half_x, top + half_y);
  }
}

void
hold()
{
  ++s_holders;
}

void
release()
{
  if (--s_holders == 0)
  {
    clear_all();
    s_blocks.clear();
    s_layers.clear();
    s_layers_frame = -1;
  }
}

void
begin_frame(const void* scene)
{
  ++s_frame;
  s_now = SDL_GetTicks();
  // Enough tile pixels for the copies a frame makes, the rest let go of
  TextureManager::current()->trim_pixels(256);
  s_scene_start = scene != s_scene || s_frame != s_scene_frame + 1;
  s_scene = scene;
  s_scene_frame = s_frame;
  s_urgent = 3.0;
  s_prepare = 4.0;
  s_prefetch = 1.0;
  if (s_generation != TextureManager::current()->get_generation())
  {
    clear_all();
    s_generation = TextureManager::current()->get_generation();
  }
  if (s_now - s_swept > 1000)
  {
    s_swept = s_now;
    sweep(KEEP);
    // A page with nothing left in it lets go of its texture
    for (Page& page : s_pages)
    {
      if (page.picture && static_cast<int>(page.free.size()) == s_per_page) page.picture.reset();
    }
  }
  // A little each frame, so pages follow the copies in use
  compact(0.25);
}

std::vector<const TileMap*>
layers_above(const GameObjectManager& objects, const TileMap& map)
{
  take_layers(objects);
  std::vector<const TileMap*> above;
  const auto it = std::find(s_layers.begin(), s_layers.end(), &map);
  if (it == s_layers.end()) return above;
  for (auto upper = it + 1; upper != s_layers.end(); ++upper)
  {
    if ((*upper)->get_layer() > map.get_layer()) above.push_back(*upper);
  }
  return above;
}

bool
layers_fading(const GameObjectManager& objects)
{
  take_layers(objects);
  return s_layers_fading;
}

void
gather_blocks(const GameObjectManager& objects, const Rectf& near)
{
  s_blocks.clear();
  for (const auto& object : objects.get_objects())
  {
    const auto* block = dynamic_cast<const Block*>(object.get());
    if (!block || !near.contains(block->get_pos()) || !block->get_resting_surface()) continue;
    const Vector pos = block->get_pos();
    s_blocks[cell_key(static_cast<int>(std::round(pos.x / 32.0f)), static_cast<int>(std::round(pos.y / 32.0f)))] = block;
  }
}

const Block*
block_at(int x, int y)
{
  const auto it = s_blocks.find(cell_key(x, y));
  return it == s_blocks.end() ? nullptr : it->second;
}

const Surface*
grid_surface_at(const GameObjectManager& objects, int x, int y)
{
  for (const TileMap* tilemap : objects.get_solid_tilemaps())
  {
    if (tilemap->get_offset() != Vector(0.0f, 0.0f) || tilemap->get_path() || id_at(*tilemap, x, y) == 0) continue;
    const Surface* surface = tilemap->get_tile(x, y).get_current_surface_ptr();
    if (surface) return surface;
  }
  const Block* block = block_at(x, y);
  return block ? block->get_resting_surface().get() : nullptr;
}

SurfacePtr
copy_of(const std::array<const Surface*, 9>& cells, Vector& origin, Size& texels)
{
  Key key;
  for (size_t c = 0; c < 9; ++c)
  {
    key.cells[c] = fitting(cells[c]);
  }
  if (!key.cells[4]) return {};
  const int slot = find_or_build(std::move(key));
  if (slot < 0) return {};
  s_slots[static_cast<size_t>(slot)].last = s_now;
  origin = origin_of(slot);
  texels = s_texels;
  return s_pages[static_cast<size_t>(slot / s_per_page)].picture;
}

Layer::Layer() :
  m_cells(),
  m_moving(),
  m_moving_index(),
  m_size(0, 0),
  m_revision(-1),
  m_generation(-1),
  m_cover()
{
}

void
Layer::refresh(const TileMap& map, const Rect& cells, const Cover& cover)
{
  // Room for the cells on screen and a ring round them, so none of them share a place
  const Size size(std::max(m_size.width, cells.get_width() + 4), std::max(m_size.height, cells.get_height() + 4));
  const int generation = TextureManager::current()->get_generation();
  if (map.get_revision() == m_revision && generation == m_generation && cover == m_cover && size == m_size &&
      m_moving.size() < 2048)
  {
    return;
  }
  m_revision = map.get_revision();
  m_generation = generation;
  m_cover = cover;
  m_size = size;
  m_cells.assign(static_cast<size_t>(m_size.width * m_size.height), Cell{ -1, -1, 0, -1, {} });
  m_moving.clear();
  m_moving_index.clear();
}

Layer::Cell&
Layer::cell_at(int x, int y)
{
  return m_cells[static_cast<size_t>((y % m_size.height) * m_size.width + x % m_size.width)];
}

bool
Layer::current(const TileMap& map, int x, int y, const Cell& cell) const
{
  if (cell.index != y * map.get_width() + x) return false;
  return cell.moving >= 0 || cell.slot < 0 ||
         (static_cast<size_t>(cell.slot) < s_slots.size() && s_slots[static_cast<size_t>(cell.slot)].stamp == cell.stamp);
}

bool
Layer::resolve(const TileMap& map, int x, int y, const Cover& cover, Cell& cell)
{
  const auto used = [&map, x, y](int dx, int dy) { return id_at(map, x + dx, y + dy) != 0; };
  cell.index = y * map.get_width() + x;
  cell.slot = -1;
  cell.moving = -1;
  cell.open = Open{ !used(-1, 0), !used(0, -1), !used(-1, -1), !used(1, -1) };

  std::array<const Tile*, 9> tiles{};
  bool animated = false;
  for (int k = 0; k < 9; ++k)
  {
    if (!used(k % 3 - 1, k / 3 - 1)) continue;
    const Tile* tile = &map.get_tile(x + k % 3 - 1, y + k / 3 - 1);
    tiles[static_cast<size_t>(k)] = tile;
    animated = animated || tile->is_animated();
  }
  Key key;
  for (size_t c = 0; c < 9; ++c)
  {
    key.cells[c] = tiles[c] ? fitting(tiles[c]->get_current_surface_ptr()) : nullptr;
  }
  if (!key.cells[4] && !animated) return true;

  // A layer over this one counts only where its edges change a texel of the copy, so the copies stay shared elsewhere
  for (const auto& layer : (key.cells[4] || animated) ? cover : Cover())
  {
    std::array<const Surface*, 9> pictures;
    bool clear = true;
    bool opaque = true;
    for (int k = 0; k < 9; ++k)
    {
      pictures[static_cast<size_t>(k)] = fitting(layer.first->get_still_picture_at(x + k % 3 - 1, y + k / 3 - 1));
      const uint8_t kind = kind_of(pictures[static_cast<size_t>(k)]);
      clear = clear && kind == CLEAR;
      opaque = opaque && kind == OPAQUE;
    }
    if (clear || opaque || !key.cells[4]) continue;
    Key alone;
    alone.cells = key.cells;
    alone.over.assign(pictures.begin(), pictures.end());
    size_copies(*key.cells[4]);
    bool changes = false;
    compute(alone, s_texels.width, s_texels.height, &changes);
    if (changes) key.over.insert(key.over.end(), pictures.begin(), pictures.end());
  }

  if (animated)
  {
    auto it = m_moving_index.find({ tiles, key.over });
    if (it == m_moving_index.end())
    {
      it = m_moving_index.emplace(std::make_pair(tiles, key.over), static_cast<int>(m_moving.size())).first;
      m_moving.push_back(Moving{ tiles, std::move(key.over), {}, 0, -1 });
    }
    cell.moving = it->second;
    return true;
  }

  cell.slot = find_or_build(std::move(key));
  if (cell.slot < 0)
  {
    cell.index = -1;
    return false;
  }
  cell.stamp = s_slots[static_cast<size_t>(cell.slot)].stamp;
  return true;
}

bool
Layer::find_shown(Moving& moving) const
{
  std::array<uint8_t, 9> frames;
  for (size_t c = 0; c < 9; ++c)
  {
    frames[c] = static_cast<uint8_t>(moving.tiles[c] ? moving.tiles[c]->get_frame() : 0);
  }
  bool found = false;
  for (size_t i = 0; i < moving.shown.size(); ++i)
  {
    const Shown& shown = moving.shown[i];
    const bool alive = shown.slot < 0 || (static_cast<size_t>(shown.slot) < s_slots.size() &&
                                          s_slots[static_cast<size_t>(shown.slot)].stamp == shown.stamp);
    if (!alive) continue;
    if (shown.slot >= 0) s_slots[static_cast<size_t>(shown.slot)].last = s_now;
    if (!found && shown.frames == frames)
    {
      moving.now = i;
      found = true;
    }
  }
  return found;
}

bool
Layer::add_shown(Moving& moving) const
{
  Shown shown;
  Key key;
  for (size_t c = 0; c < 9; ++c)
  {
    key.cells[c] = moving.tiles[c] ? fitting(moving.tiles[c]->get_current_surface_ptr()) : nullptr;
    shown.frames[c] = static_cast<uint8_t>(moving.tiles[c] ? moving.tiles[c]->get_frame() : 0);
  }
  shown.slot = -1;
  shown.stamp = 0;
  if (key.cells[4])
  {
    key.over = moving.over;
    shown.slot = find_or_build(std::move(key));
    if (shown.slot < 0) return false;
    shown.stamp = s_slots[static_cast<size_t>(shown.slot)].stamp;
  }
  // One that stopped being alive, or the oldest of a long run of frames, makes way
  auto same = std::find_if(moving.shown.begin(), moving.shown.end(), [&shown](const Shown& other) { return other.frames == shown.frames; });
  if (same != moving.shown.end()) moving.shown.erase(same);
  if (moving.shown.size() >= 64) moving.shown.erase(moving.shown.begin());
  moving.shown.push_back(shown);
  moving.now = moving.shown.size() - 1;
  return true;
}

bool
Layer::draw(const TileMap& map, const Rect& cells, const std::vector<const TileMap*>& above,
            std::vector<Draw>& draws, std::vector<int>& loose)
{
  const Cover cover = cover_of(above);
  refresh(map, cells, cover);

  // Which of draws is each page's
  std::vector<int> drawn;
  const Sizef size(32.0f, 32.0f);
  for (int y = cells.top; y < cells.bottom; ++y)
  {
    for (int x = cells.left; x < cells.right; ++x)
    {
      if (map.get_tile_id(x, y) == 0) continue;
      Cell& cell = cell_at(x, y);
      if (!current(map, x, y, cell))
      {
        // All of them ready or the whole layer goes tile by tile, as copies beside single tiles would show their joints
        if (!s_scene_start && s_urgent <= 0.0) return false;
        bool room = true;
        timed(s_urgent, [&] { room = resolve(map, x, y, cover, cell); });
        if (!room) return false;
      }
      int slot = cell.slot;
      if (cell.moving >= 0)
      {
        Moving& moving = m_moving[static_cast<size_t>(cell.moving)];
        if (moving.checked != s_frame && !find_shown(moving))
        {
          if (!s_scene_start && s_urgent <= 0.0) return false;
          bool room = true;
          timed(s_urgent, [&] { room = add_shown(moving); });
          if (!room) return false;
        }
        moving.checked = s_frame;
        slot = moving.shown[moving.now].slot;
      }
      if (slot < 0)
      {
        loose.push_back(y * map.get_width() + x);
        continue;
      }
      s_slots[static_cast<size_t>(slot)].last = s_now;
      const size_t page = static_cast<size_t>(slot / s_per_page);
      if (page >= drawn.size()) drawn.resize(page + 1, -1);
      if (drawn[page] < 0)
      {
        drawn[page] = static_cast<int>(draws.size());
        draws.push_back(Draw{ s_pages[page].picture, {}, {} });
      }
      Draw& draw = draws[static_cast<size_t>(drawn[page])];
      place(map.get_tile_position(x, y), size, origin_of(slot), s_texels.width, s_texels.height, cell.open,
            draw.srcrects, draw.dstrects);
    }
  }
  prefetch(map, cells, cover);
  return true;
}

void
Layer::prefetch(const TileMap& map, const Rect& cells, const Cover& cover)
{
  const Rect ring(std::max(cells.left - 1, 0), std::max(cells.top - 1, 0),
                  std::min(cells.right + 1, map.get_width()), std::min(cells.bottom + 1, map.get_height()));
  for (int y = ring.top; y < ring.bottom; ++y)
  {
    for (int x = ring.left; x < ring.right; ++x)
    {
      if (y >= cells.top && y < cells.bottom && x == cells.left) x = cells.right;
      if (x >= ring.right) break;
      if (map.get_tile_id(x, y) == 0) continue;
      Cell& cell = cell_at(x, y);
      if (!current(map, x, y, cell))
      {
        if (s_prefetch <= 0.0) return;
        bool room = true;
        timed(s_prefetch, [&] { room = resolve(map, x, y, cover, cell); });
        if (!room) return;
      }
      if (cell.slot >= 0) s_slots[static_cast<size_t>(cell.slot)].last = s_now;
    }
  }
}

void
Layer::prepare(const TileMap& map, const Rect& cells, const std::vector<const TileMap*>& above)
{
  const Cover cover = cover_after(above);
  refresh(map, cells, cover);
  for (int y = cells.top; y < cells.bottom; ++y)
  {
    for (int x = cells.left; x < cells.right; ++x)
    {
      if (map.get_tile_id(x, y) == 0) continue;
      Cell& cell = cell_at(x, y);
      if (!current(map, x, y, cell))
      {
        if (s_prepare <= 0.0) return;
        bool room = true;
        timed(s_prepare, [&] { room = resolve(map, x, y, cover, cell); });
        if (!room) return;
      }
      if (cell.slot >= 0) s_slots[static_cast<size_t>(cell.slot)].last = s_now;
    }
  }
}

} // namespace bordered

/* EOF */
