// tests/arena_test.cpp
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

#include "st_assert.hpp"

#include <cstddef>
#include <cstdint>
#include <new>
#include <vector>

#include "util/arena.hpp"

namespace {

bool is_aligned(const void* p)
{
  return (reinterpret_cast<std::uintptr_t>(p) % alignof(std::max_align_t)) == 0;
}

/** Fills each allocation with a mark of its own and reads them all back, so an
    overlap or a short block shows up as a wrong byte rather than a crash. */
void check_no_overlap(size_t count, size_t bytes)
{
  Arena arena;
  std::vector<char*> blocks;

  for (size_t i = 0; i < count; ++i)
  {
    char* p = static_cast<char*>(arena.allocate(bytes));
    ST_ASSERT(p != nullptr);
    ST_ASSERT(is_aligned(p));
    for (size_t j = 0; j < bytes; ++j)
      p[j] = static_cast<char>(i & 0xff);
    blocks.push_back(p);
  }

  for (size_t i = 0; i < count; ++i)
    for (size_t j = 0; j < bytes; ++j)
      ST_ASSERT_EQ(static_cast<char>(i & 0xff), blocks[i][j]);
}

} // namespace

int main()
{
  // every allocation is aligned for any type
  {
    Arena arena;
    for (size_t bytes = 1; bytes <= 64; ++bytes)
      ST_ASSERT(is_aligned(arena.allocate(bytes)));
  }

  // a request for nothing still gets an address, and not a shared one
  {
    Arena arena;
    void* a = arena.allocate(0);
    void* b = arena.allocate(0);
    ST_ASSERT(a != nullptr);
    ST_ASSERT(b != nullptr);
    ST_ASSERT(a != b);
  }

  // successive allocations do not tread on each other, inside one block
  check_no_overlap(64, 32);

  // nor across the block boundary, which this crosses several times over
  check_no_overlap(4096, 1024);

  // a single allocation larger than a whole block is served
  {
    Arena arena;
    const size_t big = 2u * 1024u * 1024u;
    char* p = static_cast<char*>(arena.allocate(big));
    ST_ASSERT(p != nullptr);
    ST_ASSERT(is_aligned(p));
    p[0] = 'a';
    p[big - 1] = 'z';
    ST_ASSERT_EQ('a', p[0]);
    ST_ASSERT_EQ('z', p[big - 1]);
  }

  // a reset keeps the memory and hands the same address back out again, which
  // is the whole reason this exists rather than a malloc for every request
  {
    Arena arena;
    void* first = arena.allocate(48);
    arena.reset();
    ST_ASSERT_EQ(first, arena.allocate(48));
  }

  // a run wider than one block settles down: once the arena has grown to fit
  // the most it is asked for, it stops moving and hands back the same memory
  {
    Arena arena;
    void* settled = nullptr;

    for (size_t round = 0; round < 8; ++round)
    {
      void* first = nullptr;
      for (size_t i = 0; i < 2048; ++i)
      {
        void* p = arena.allocate(64);
        ST_ASSERT(p != nullptr);
        ST_ASSERT(is_aligned(p));
        if (i == 0) first = p;
      }
      arena.reset();

      // the first round grows the arena, so only compare once it has grown
      if (round == 1) settled = first;
      if (round > 1) ST_ASSERT_EQ(settled, first);
    }
  }

  // a size that would wrap when rounded up is refused rather than served short
  {
    Arena arena;
    ST_ASSERT_THROW(arena.allocate(static_cast<size_t>(-1)), std::bad_alloc);
  }

  // placement new reaches the arena and the object lands aligned
  {
    Arena arena;
    struct Payload { double a; void* b; int c; };
    Payload* p = new (arena) Payload();
    ST_ASSERT(p != nullptr);
    ST_ASSERT(is_aligned(p));
    p->c = 7;
    ST_ASSERT_EQ(7, p->c);
    p->~Payload();
  }

  return 0;
}

/* EOF */
