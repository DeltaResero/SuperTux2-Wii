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

#include "util/log.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace FileSystem {

namespace {

std::vector<std::string> s_search_paths;
std::string s_write_dir;

/** normalize() yields a leading slash and callers pass that straight back in,
    meaning "at the root of a search path entry" rather than at the host root.
    Strip it before joining, or every such name looks absolute. */
std::string strip_root(const std::string& filename)
{
  std::string::size_type start = filename.find_first_not_of('/');
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
    const std::string dir = relative.empty() ? base : join(base, relative);

    std::error_code ec;
    std::filesystem::directory_iterator it(dir, ec);
    if (ec) continue;

    for (const auto& entry : it)
    {
      std::string name = entry.path().filename().string();
      if (std::find(names.begin(), names.end(), name) == names.end())
        names.push_back(std::move(name));
    }
  }

  return names;
}

/* These three take real host paths. Everything below them manipulates the
   virtual paths the search path is addressed with, where the separator is
   always '/' and std::filesystem's host semantics would be wrong. */

bool exists(const std::string& path)
{
  std::error_code ec;
  return std::filesystem::exists(path, ec);
}

bool is_directory(const std::string& path)
{
  std::error_code ec;
  return std::filesystem::is_directory(path, ec);
}

void mkdir(const std::string& directory)
{
  std::error_code ec;
  if (!std::filesystem::create_directory(directory, ec))
  {
    throw std::runtime_error("failed to create directory: "  + directory);
  }
}

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    p = filename.find_last_of('\\');
  if(p == std::string::npos)
    return "./";

  return filename.substr(0, p+1);
}

std::string basename(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    p = filename.find_last_of('\\');
  if(p == std::string::npos)
    return filename;

  return filename.substr(p+1, filename.size()-p-1);
}

std::string strip_extension(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('.');
  if(p == std::string::npos)
    return filename;

  return filename.substr(0, p);
}

std::string normalize(const std::string& filename)
{
  std::vector<std::string> path_stack;

  const char* p = filename.c_str();

  while(true) {
    while(*p == '/' || *p == '\\') {
      p++;
      continue;
    }

    const char* pstart = p;
    while(*p != '/' && *p != '\\' && *p != 0) {
      ++p;
    }

    size_t len = p - pstart;
    if(len == 0)
      break;

    std::string pathelem(pstart, p-pstart);
    if(pathelem == ".")
      continue;

    if(pathelem == "..") {
      if(path_stack.empty()) {

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
  for(std::vector<std::string>::iterator i = path_stack.begin();
      i != path_stack.end(); ++i) {
    result << '/' << *i;
  }
  if(path_stack.empty())
    result << '/';

  return result.str();
}

std::string join(const std::string& lhs, const std::string& rhs)
{
  if (lhs.empty())
  {
    return rhs;
  }
  else if (lhs.back() == '/')
  {
    return lhs + rhs;
  }
  else
  {
    return lhs + "/" + rhs;
  }
}

} // namespace FileSystem

/* EOF */
