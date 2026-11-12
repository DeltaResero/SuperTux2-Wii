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

/** An object that can drop its pictures while another screen covers it and fetch them again after.
    It isn't drawn in between, but anything else it does has to cope with having none. */
class ArtworkInterface
{
public:
  virtual ~ArtworkInterface() {}

  /** Lets go of the pictures */
  virtual void release_artwork() = 0;

  /** Fetches them again and leaves the object looking as release found it */
  virtual void reacquire_artwork() = 0;
};

#endif

/* EOF */
