// src/physfs/ofile_stream.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_PHYSFS_OFILE_STREAM_CPP
#define HEADER_SUPERTUX_PHYSFS_OFILE_STREAM_CPP

#include "physfs/ofile_stream.hpp"

#include "physfs/ofile_streambuf.hpp"

OFileStream::OFileStream(const std::string& filename) :
  std::ostream(nullptr), sb(new OFileStreambuf(filename))
{
  init(sb.get());
}

OFileStream::~OFileStream()
{
}

#endif

// EOF
