// src/util/file_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2025 DeltaResero
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_FILE_SYSTEM_HPP
#define HEADER_SUPERTUX_UTIL_FILE_SYSTEM_HPP

#include <string>

namespace FileSystem {

/** Initialize the filesystem (find user dir, data dir) */
void init(const char* argv0);

/** Find a file in the search paths. Returns absolute path or empty string. */
std::string find(const std::string& filename);

/** Get the writable user directory */
std::string get_user_dir();

/** Returns true if the given path is a directory */
bool is_directory(const std::string& path);

/** Return true if the given file exists */
bool exists(const std::string& path);

/** Create the given directory in the user dir */
void mkdir(const std::string& directory);

/** returns the path of the directory the file is in */
std::string dirname(const std::string& filename);

/** returns the name of the file */
std::string basename(const std::string& filename);

/** remove everything starting from and including the last dot */
std::string strip_extension(const std::string& filename);

/** normalize filename */
std::string normalize(const std::string& filename);

/** join two filenames */
std::string join(const std::string& lhs, const std::string& rhs);

} // namespace FileSystem

#endif // HEADER_SUPERTUX_UTIL_FILE_SYSTEM_HPP

// EOF
