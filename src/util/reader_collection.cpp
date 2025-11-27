// src/util/reader_collection.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "util/reader_collection.hpp"

#include <assert.h>
#include <sexp/value.hpp>

#include "util/reader_error.hpp"

ReaderCollection::ReaderCollection(const ReaderDocument* doc, const sexp::Value* sx) :
  m_doc(doc),
  m_sx(sx)
{
  assert(m_doc);
  assert(m_sx);
}

ReaderCollection::ReaderCollection() :
  m_doc(nullptr),
  m_sx(nullptr)
{
}

std::vector<ReaderObject>
ReaderCollection::get_objects() const
{
  assert(m_doc);
  assert(m_sx);

  assert_is_array(*m_doc, *m_sx);

  std::vector<ReaderObject> result;
  auto const& arr = m_sx->as_array();
  for(size_t i = 1; i < arr.size(); ++i)
  {
    result.push_back(ReaderObject(m_doc, &arr[i]));
  }
  return result;
}

// EOF
