// src/physfs/ifile_stream.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "physfs/ifile_stream.hpp"

#include "physfs/ifile_streambuf.hpp"

IFileStream::IFileStream(const std::string& filename) :
  std::istream(nullptr), sb(new IFileStreambuf(filename))
{
  init(sb.get());
}

IFileStream::~IFileStream()
{
}

// EOF
