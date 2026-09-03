// src/supertux/artwork_interface.hpp
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

#ifndef HEADER_SUPERTUX_SUPERTUX_ARTWORK_INTERFACE_HPP
#define HEADER_SUPERTUX_SUPERTUX_ARTWORK_INTERFACE_HPP

/**
 * Objects that can drop their pictures while another screen is up and fetch
 * them again after. Callers promise not to draw a released object, not to
 * leave it alone, so anything outside draw() must cope with no sprite.
 */
class ArtworkInterface
{
public:
  virtual ~ArtworkInterface()
  {}

  /** Let go of the pictures. */
  virtual void release_artwork() = 0;

  /** Fetch them again, leaving the object as release found it. */
  virtual void reacquire_artwork() = 0;
};

#endif

/* EOF */
