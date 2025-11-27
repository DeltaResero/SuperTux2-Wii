// src/physfs/ofile_stream.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_PHYSFS_OFILE_STREAM_HPP
#define HEADER_SUPERTUX_PHYSFS_OFILE_STREAM_HPP

#include <memory>
#include <ostream>
#include <physfs.h>

class OFileStream : public std::ostream
{
protected:
  std::unique_ptr<std::streambuf> sb;

public:
  OFileStream(const std::string& filename);
  ~OFileStream();

private:
  OFileStream(const OFileStream&) = delete;
  OFileStream& operator=(const OFileStream&) = delete;
};

#endif // HEADER_SUPERTUX_PHYSFS_OFILE_STREAM_HPP

// EOF
