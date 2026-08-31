// src/util/file_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_UTIL_FILE_SYSTEM_HPP
#define HEADER_SUPERTUX_UTIL_FILE_SYSTEM_HPP

#include <string>
#include <vector>

namespace FileSystem {

/* The search path. Game data is addressed by a name relative to one of these
   directories, such as "levels/world1/worldmap.stwm", and find() turns such a
   name into a real path. Entries are searched in the order they were added, so
   an earlier one shadows a later one. */

/** Add a directory to the end of the search path, or to the front when
    prepend is set. */
void add_search_path(const std::string& directory, bool prepend = false);

/** Drop every search path entry. */
void clear_search_paths();

/** The search path, in the order find() walks it. */
std::vector<std::string> get_search_paths();

/** Set the directory new files are written to. It is not added to the search
    path; callers that want it searched too must add it. */
void set_write_dir(const std::string& directory);
std::string get_write_dir();

/** Turn a search-path-relative name into a real path, or return an empty
    string when no entry holds it. An absolute name is returned unchanged if
    it exists. */
std::string find(const std::string& filename);

/** Where a file of this name would be written. Empty if no write directory
    has been set. */
std::string write_path(const std::string& filename);

/** The names directly inside a search-path-relative directory, gathered from
    every entry that has one, each name appearing once. */
std::vector<std::string> enumerate(const std::string& directory);

/* The rest take real paths, as returned by find(), not search-path-relative
   names. */

/** Returns true if the given path is a directory */
bool is_directory(const std::string& path);

/** Return true if the given file exists */
bool exists(const std::string& path);

/**
 *  Create the given directory and any parent it needs. Does nothing if it is
 *  already there; throws with the reason if it cannot be made.
 */
void mkdir(const std::string& directory);

/**
 * returns the path of the directory the file is in
 */
std::string dirname(const std::string& filename);

/**
 * returns the name of the file
 */
std::string basename(const std::string& filename);

/**
 * remove everything starting from and including the last dot
 */
std::string strip_extension(const std::string& filename);

/**
 * normalize filename so that "blup/bla/blo/../../bar" will become
 * "blup/bar"
 */
std::string normalize(const std::string& filename);

/**
 * join two filenames join("foo", "bar") -> "foo/bar"
 */
std::string join(const std::string& lhs, const std::string& rhs);

} // namespace FileSystem

#endif

/* EOF */
