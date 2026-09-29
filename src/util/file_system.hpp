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

/* Game data is named relative to the search path; find() turns a name into a real path. */

/** Add a directory to the end of the search path, or the front if prepend is set */
void add_search_path(const std::string& directory, bool prepend = false);

/** Drop every search path entry */
void clear_search_paths();

/** The search path, in the order find() walks it */
std::vector<std::string> get_search_paths();

/** Set the directory new files are written to; it is not searched unless added too */
void set_write_dir(const std::string& directory);
std::string get_write_dir();

/** Real path of a search-path-relative name, or empty when no entry holds it */
std::string find(const std::string& filename);

/** Real path a file of this name is written to, or empty with no write directory */
std::string write_path(const std::string& filename);

/** Names inside a search-path-relative directory from every entry, sorted and unique */
std::vector<std::string> enumerate(const std::string& directory);

/* The functions below take real paths, not search-path-relative names. */

/** Returns true if the given path is a directory */
bool is_directory(const std::string& path);

/** Return true if the given file exists */
bool exists(const std::string& path);

/** Create the given directory */
void mkdir(const std::string& directory);

/** returns the path of the directory the file is in */
std::string dirname(const std::string& filename);

/** returns the name of the file */
std::string basename(const std::string& filename);

/** normalize filename so that "blup/bla/blo/../../bar" will become
    "blup/bar" */
std::string normalize(const std::string& filename);

/** join two filenames join("foo", "bar") -> "foo/bar" */
std::string join(const std::string& lhs, const std::string& rhs);

} // namespace FileSystem

#endif

/* EOF */
