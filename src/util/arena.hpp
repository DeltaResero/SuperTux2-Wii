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

#ifndef HEADER_SUPERTUX_UTIL_ARENA_HPP
#define HEADER_SUPERTUX_UTIL_ARENA_HPP

#include <cstddef>

/** Hands out memory by moving a pointer along and takes it all back at once,
    keeping its blocks so a full reset every frame stops asking for more. */
class Arena final
{
public:
  Arena();
  ~Arena();

  /** Aligned for any type, never null; adds a block when the current one runs out */
  void* allocate(size_t bytes);

  /** Runs no destructors, so anything holding a resource goes first */
  void reset();

private:
  struct Block
  {
    Block* next;
    char* data;
    size_t size;
    size_t used;
  };

  void add_block(size_t bytes);
  // Folds a grown chain into one block, so a reset starts from all of it
  void consolidate();
  static void free_blocks(Block* block);

private:
  Block* m_blocks; /**< Newest first */
  Block* m_current;
  size_t m_next_size;

private:
  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;
};

inline void*
operator new(size_t bytes, Arena& arena)
{
  return arena.allocate(bytes);
}

inline void*
operator new[](size_t bytes, Arena& arena)
{
  return arena.allocate(bytes);
}

#endif

/* EOF */
