// src/util/wii.hpp
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

#ifndef HEADER_SUPERTUX_UTIL_WII_HPP
#define HEADER_SUPERTUX_UTIL_WII_HPP

#ifdef __wii__

#include <string>

namespace Wii {

/* Storage is mounted on demand rather than at startup, so a console with no
   card in it only pays for the drive it actually has. Each mount is idempotent
   and reports whether the device is usable afterwards. */

/** Mount the SD card. */
bool mount_sd();

/** Release the SD card, if it was mounted. */
void unmount_sd();

/** Mount the first USB storage device. */
bool mount_usb();

/** Release the USB storage device, if it was mounted. */
void unmount_usb();

/** The directory the game was installed into, USB searched before SD.
    Throws std::runtime_error when neither device holds an installation. */
std::string get_base_dir();

/** The data directory within that installation.
    Throws std::runtime_error when it is missing. */
std::string get_data_dir();

/** Where saves and configuration are written. The caller creates it, so this
    names a directory that need not exist yet. */
std::string get_user_dir();

} // namespace Wii

#endif

#endif

/* EOF */
