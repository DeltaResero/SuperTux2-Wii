// src/util/file_system.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "util/file_system.hpp"

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <SDL.h>

#include "util/log.hpp"

namespace fs = std::filesystem;

namespace FileSystem {

static std::vector<fs::path> search_paths;
static fs::path user_dir;

void init(const char* argv0) {
  search_paths.clear();

  // Determine data directory first (most important for reading game assets)
  fs::path data_dir;
  bool found_data = false;

  // Check environment variable
  if (const char* env_datadir = getenv("SUPERTUX2_DATA_DIR")) {
    data_dir = fs::path(env_datadir);
    if (fs::exists(data_dir) && fs::is_directory(data_dir)) {
      found_data = true;
      log_info << "Data Directory (from env): " << data_dir.string() << std::endl;
    }
  }

  // If not found, check relative to executable
  if (!found_data) {
    char* base_path = SDL_GetBasePath();
    if (base_path) {
      fs::path exe_path(base_path);
      SDL_free(base_path);

      // Check common locations relative to executable
      std::vector<fs::path> potential_paths = {
        exe_path / "data",                              // ./data
        exe_path.parent_path() / "data",                // ../data
        exe_path / ".." / "share" / "supertux2",        // ../share/supertux2
        exe_path / ".." / ".." / "share" / "supertux2", // ../../share/supertux2
      };

      for (const auto& p : potential_paths) {
        fs::path normalized = p.lexically_normal();
        // Check if this path contains the data directory structure
        if (fs::exists(normalized) && fs::is_directory(normalized)) {
          // Verify it's actually a data directory by checking for a known file
          if (fs::exists(normalized / "images") || fs::exists(normalized / "credits.stxt")) {
            data_dir = normalized;
            found_data = true;
            log_info << "Data Directory (found): " << data_dir.string() << std::endl;
            break;
          }
        }
      }
    }
  }

  // Fallback: check current working directory
  if (!found_data) {
    fs::path cwd = fs::current_path();
    std::vector<fs::path> cwd_paths = {
      cwd / "data",
      cwd
    };

    for (const auto& p : cwd_paths) {
      if (fs::exists(p) && fs::is_directory(p)) {
        if (fs::exists(p / "images") || fs::exists(p / "credits.stxt")) {
          data_dir = p;
          found_data = true;
          log_info << "Data Directory (cwd): " << data_dir.string() << std::endl;
          break;
        }
      }
    }
  }

  if (found_data) {
    search_paths.push_back(data_dir);
  } else {
    log_warning << "Could not find data directory! Game assets will not load correctly." << std::endl;
  }

  // User Directory (Write dir) - add second so user files can override
  // Use the old PhysFS-compatible location for backwards compatibility
#ifdef _WIN32
  // Windows: Use AppData/SuperTux/supertux2
  char* pref_path = SDL_GetPrefPath("SuperTux", "supertux2");
  if (pref_path) {
    user_dir = fs::path(pref_path);
    SDL_free(pref_path);
  }
#else
  // Linux/Unix/Wii: Use ~/.supertux2 (PhysFS compatible)
  const char* home = getenv("HOME");
  if (home) {
    user_dir = fs::path(home) / ".supertux2";
  } else {
    // Fallback to SDL_GetPrefPath if HOME not set
    char* pref_path = SDL_GetPrefPath("SuperTux", "supertux2");
    if (pref_path) {
      user_dir = fs::path(pref_path);
      SDL_free(pref_path);
    }
  }
#endif

  if (!user_dir.empty()) {
    if (!fs::exists(user_dir)) {
      fs::create_directories(user_dir);
      log_info << "Created user directory: " << user_dir.string() << std::endl;
    }
    search_paths.push_back(user_dir);
    log_info << "User Directory: " << user_dir.string() << std::endl;
  } else {
    log_warning << "Could not determine user directory!" << std::endl;
  }

  // Debug: print all search paths
  log_info << "Search paths initialized (" << search_paths.size() << " paths):" << std::endl;
  for (size_t i = 0; i < search_paths.size(); ++i) {
    log_info << "  [" << i << "] " << search_paths[i].string() << std::endl;
  }
}

std::string find(const std::string& filename) {
  // Handle empty filename
  if (filename.empty()) {
    return "";
  }

  fs::path p(filename);

  // If it's an absolute path, check it directly
  if (p.is_absolute()) {
    if (fs::exists(p)) {
      return p.string();
    }
    return "";
  }

  // Search through all search paths
  for (const auto& base : search_paths) {
    fs::path full_path = base / p;
    // Normalize the path to handle .. and . components
    full_path = full_path.lexically_normal();

    if (fs::exists(full_path)) {
      return full_path.string();
    }
  }

  return "";
}

std::string get_user_dir() {
  return user_dir.string();
}

bool exists(const std::string& path) {
  return !find(path).empty();
}

bool is_directory(const std::string& path) {
  std::string real_path = find(path);
  if (real_path.empty()) return false;
  return fs::is_directory(real_path);
}

void mkdir(const std::string& directory) {
  if (user_dir.empty()) {
    log_warning << "Cannot create directory: user_dir not initialized" << std::endl;
    return;
  }
  fs::path p = user_dir / directory;
  fs::create_directories(p);
}

std::string dirname(const std::string& filename) {
  fs::path p(filename);
  std::string parent = p.parent_path().string();
  // Return empty string instead of "." for files with no directory
  return (parent == ".") ? "" : parent;
}

std::string basename(const std::string& filename) {
  fs::path p(filename);
  return p.filename().string();
}

std::string strip_extension(const std::string& filename) {
  fs::path p(filename);
  return p.stem().string();
}

std::string normalize(const std::string& filename) {
  fs::path p(filename);
  return p.lexically_normal().string();
}

std::string join(const std::string& lhs, const std::string& rhs) {
  if (lhs.empty()) return rhs;
  if (rhs.empty()) return lhs;
  fs::path p(lhs);
  return (p / rhs).string();
}

} // namespace FileSystem

// EOF
