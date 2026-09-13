// src/util/arena.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include <stddef.h>

/** Hands out memory by moving a pointer along, and takes it all back in one
    call. Blocks are kept rather than returned, so a caller that fills the arena
    and resets it every frame stops asking the system for memory once it has
    reached the most it needs at once. */
class Arena final
{
public:
  Arena();
  ~Arena();

  /** Aligned for any type and never null; takes another block when the one in
      hand runs out. */
  void* allocate(size_t bytes);

  /** Hands every block back to the arena. Runs no destructor, so anything left
      holding a resource has to be destroyed before this is called. */
  void reset();

private:
  struct Block
  {
    Block* next;
    char* data;
    size_t size;
    size_t used;
  };

  /** Newest first, so a reset starts from the largest block. */
  Block* blocks;

  /** Where the next allocation comes from. */
  Block* current;

  /** How big the block after this one will be. */
  size_t next_size;

  void add_block(size_t bytes);

  /** Folds a grown chain back into one block, so nothing sits unreachable
      behind the block a reset starts from. */
  void consolidate();

  static void free_blocks(Block* block);

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;
};

inline void*
operator new (size_t bytes, Arena& arena)
{
  return arena.allocate(bytes);
}

inline void*
operator new[] (size_t bytes, Arena& arena)
{
  return arena.allocate(bytes);
}

#endif

/* EOF */
