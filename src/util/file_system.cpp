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

#include "util/file_system.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "gui/dialog.hpp"
#include "util/log.hpp"
#include "util/string_util.hpp"

namespace fs = std::filesystem;

namespace FileSystem {

namespace {

std::vector<std::string> s_search_paths;
std::string s_write_dir;

/** A leading slash means the root of a search path entry, not of the host */
std::string strip_root(const std::string& filename)
{
  const std::string::size_type start = filename.find_first_not_of('/');
  if (start == std::string::npos) return std::string();
  return filename.substr(start);
}

} // namespace

void add_search_path(const std::string& directory, bool prepend)
{
  if (prepend)
    s_search_paths.insert(s_search_paths.begin(), directory);
  else
    s_search_paths.push_back(directory);
}

void clear_search_paths()
{
  s_search_paths.clear();
}

std::vector<std::string> get_search_paths()
{
  return s_search_paths;
}

void set_write_dir(const std::string& directory)
{
  s_write_dir = directory;
}

std::string get_write_dir()
{
  return s_write_dir;
}

std::string find(const std::string& filename)
{
  const std::string relative = strip_root(filename);
  if (relative.empty()) return std::string();

  for (const auto& base : s_search_paths)
  {
    const std::string candidate = join(base, relative);
    if (exists(candidate)) return candidate;
  }
  return std::string();
}

std::string write_path(const std::string& filename)
{
  if (s_write_dir.empty()) return std::string();
  return join(s_write_dir, strip_root(filename));
}

std::vector<std::string> enumerate(const std::string& directory)
{
  const std::string relative = strip_root(directory);
  std::vector<std::string> names;

  for (const auto& base : s_search_paths)
  {
    std::error_code ec;
    fs::directory_iterator it(relative.empty() ? base : join(base, relative), ec);
    if (ec) continue;

    for (const auto& entry : it)
      names.push_back(entry.path().filename().string());
  }

  std::sort(names.begin(), names.end());
  names.erase(std::unique(names.begin(), names.end()), names.end());
  return names;
}

bool exists(const std::string& path)
{
  fs::path location(path);
  std::error_code ec;

  // If we get an error (such as "Permission denied"), then ignore it
  // and pretend that the path doesn't exist.
  return fs::exists(location, ec);
}

bool is_directory(const std::string& path)
{
  fs::path location(path);
  return fs::is_directory(location);
}

void mkdir(const std::string& directory)
{
  fs::path location(directory);
  if (!fs::create_directory(location))
  {
    throw std::runtime_error("failed to create directory: "  + directory);
  }
}

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if (p == std::string::npos)
    p = filename.find_last_of('\\');
  if (p == std::string::npos)
    return "./";

  return filename.substr(0, p+1);
}

std::string basename(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if (p == std::string::npos)
    p = filename.find_last_of('\\');
  if (p == std::string::npos)
    return filename;

  return filename.substr(p+1, filename.size()-p-1);
}

std::string normalize(const std::string& filename)
{
  std::vector<std::string> path_stack;

  const char* p = filename.c_str();

  while (true) {
    while (*p == '/' || *p == '\\') {
      p++;
      continue;
    }

    const char* pstart = p;
    while (*p != '/' && *p != '\\' && *p != 0) {
      ++p;
    }

    size_t len = p - pstart;
    if (len == 0)
      break;

    std::string pathelem(pstart, p-pstart);
    if (pathelem == ".")
      continue;

    if (pathelem == "..") {
      if (path_stack.empty()) {

        log_warning << "Invalid '..' in path '" << filename << "'" << std::endl;
        // push it into the result path so that the user sees his error...
        path_stack.push_back(pathelem);
      } else {
        path_stack.pop_back();
      }
    } else {
      path_stack.push_back(pathelem);
    }
  }

  // construct path
  std::ostringstream result;
  for (std::vector<std::string>::iterator i = path_stack.begin();
       i != path_stack.end(); ++i) {
    result << '/' << *i;
  }
  if (path_stack.empty())
    result << '/';

  return result.str();
}

std::string join(const std::string& lhs, const std::string& rhs)
{
  if (lhs.empty())
  {
    return rhs;
  }
  else if (rhs.empty())
  {
    return lhs + "/";
  }
  else if (lhs.back() == '/' && rhs.front() != '/')
  {
    return lhs + rhs;
  }
  else if (lhs.back() != '/' && rhs.front() == '/')
  {
    return lhs + rhs;
  }
  else if (lhs.back() == '/' && rhs.front() == '/')
  {
    return lhs + rhs.substr(1);
  }
  else
  {
    return lhs + "/" + rhs;
  }
}

} // namespace FileSystem

/* EOF */
