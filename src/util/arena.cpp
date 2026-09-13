// src/util/arena.cpp
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

#include "util/arena.hpp"

#include <cstddef>
#include <new>

namespace {

const size_t ALIGNMENT = alignof(std::max_align_t);

const size_t FIRST_BLOCK = 64u * 1024u;

/** Stops the doubling from running away, and from wrapping. */
const size_t LARGEST_BLOCK = 4u * 1024u * 1024u;

} // namespace

Arena::Arena() :
  blocks(nullptr),
  current(nullptr),
  next_size(FIRST_BLOCK)
{
}

Arena::~Arena()
{
  free_blocks(blocks);
}

void
Arena::free_blocks(Block* block)
{
  while(block)
  {
    Block* next = block->next;
    delete[] block->data;
    delete block;
    block = next;
  }
}

void*
Arena::allocate(size_t bytes)
{
  //rounding up must not wrap, or the block would come out smaller than the fill
  if(bytes > (size_t)-1 - ALIGNMENT)
    throw std::bad_alloc();

  //a request for nothing still has to come back with an address of its own
  const size_t wanted = ((bytes ? bytes : 1) + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

  for(Block* block = current; block; block = block->next)
  {
    if(block->size - block->used >= wanted)
    {
      char* p = block->data + block->used;
      block->used += wanted;
      current = block;
      return p;
    }
  }

  add_block(wanted);

  current->used = wanted;
  return current->data;
}

void
Arena::reset()
{
  if(blocks && blocks->next)
    consolidate();

  for(Block* block = blocks; block; block = block->next)
    block->used = 0;

  current = blocks;
}

void
Arena::consolidate()
{
  size_t total = 0;
  for(Block* block = blocks; block; block = block->next)
    total += block->size;

  //the replacement is taken before the old chain goes, so a refusal here leaves
  //the arena working with what it already had
  Block* block = new (std::nothrow) Block;
  if(!block)
    return;

  block->data = new (std::nothrow) char[total];
  if(!block->data)
  {
    delete block;
    return;
  }

  block->next = nullptr;
  block->size = total;
  block->used = 0;

  free_blocks(blocks);

  blocks = block;
  current = block;
  next_size = (total < LARGEST_BLOCK / 2) ? total * 2 : LARGEST_BLOCK;
}

void
Arena::add_block(size_t bytes)
{
  const size_t size = (next_size > bytes) ? next_size : bytes;

  Block* block = new Block;
  try
  {
    block->data = new char[size];
  }
  catch(...)
  {
    delete block;
    throw;
  }

  block->next = blocks;
  block->size = size;
  block->used = 0;

  blocks = block;
  current = block;
  next_size = (size < LARGEST_BLOCK / 2) ? size * 2 : LARGEST_BLOCK;
}

/* EOF */
