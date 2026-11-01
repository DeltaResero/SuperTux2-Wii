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

#include "video/video_system.hpp"

#include <algorithm>
#include <cassert>
#include <exception>
#include <stdexcept>
#include <config.h>

#include "util/log.hpp"
#include "video/null/null_video_system.hpp"
#include "video/sdl/sdl_video_system.hpp"

#ifdef HAVE_OPENGL
#  include "video/gl/gl_video_system.hpp"
#endif

namespace {

// What the game was started with, as asked for and as it turned out
VideoSystem::Enum s_requested = VideoSystem::VIDEO_AUTO;
VideoSystem::Enum s_running = VideoSystem::VIDEO_AUTO;
std::vector<VideoSystem::Enum> s_unavailable;

} // namespace

std::unique_ptr<VideoSystem>
VideoSystem::create(VideoSystem::Enum video_system)
{
  s_requested = video_system;
  s_unavailable.clear();

  if (video_system == VIDEO_NULL)
  {
    s_running = VIDEO_NULL;
    return std::make_unique<NullVideoSystem>();
  }

#ifdef HAVE_OPENGL
  // A renderer the driver refuses steps down to the next one, the way Auto always has
  std::vector<Enum> order;
  if (video_system == VIDEO_AUTO || video_system == VIDEO_OPENGL33CORE)
  {
    order.push_back(VIDEO_OPENGL33CORE);
  }
  if (video_system != VIDEO_SDL)
  {
    order.push_back(VIDEO_OPENGL20);
  }

  for (size_t i = 0; i < order.size(); ++i)
  {
    try
    {
      auto result = std::make_unique<GLVideoSystem>(order[i] == VIDEO_OPENGL33CORE);
      s_running = order[i];
      return result;
    }
    catch(std::exception& err)
    {
      const std::string next = (i + 1 < order.size()) ? get_video_string(order[i + 1]) : "sdl";
      log_warning << "Error creating the " << get_video_string(order[i]) << " renderer, using " << next << " instead: " << err.what() << std::endl;
      s_unavailable.push_back(order[i]);
    }
  }
#else
  if (video_system == VIDEO_OPENGL33CORE || video_system == VIDEO_OPENGL20)
  {
    log_warning << "OpenGL requested, but missing using SDL fallback" << std::endl;
  }
#endif

  log_info << "new SDL renderer\n";
  s_running = VIDEO_SDL;
  return std::make_unique<SDLVideoSystem>();
}

VideoSystem::Enum
VideoSystem::get_requested_video_system()
{
  return s_requested;
}

VideoSystem::Enum
VideoSystem::get_running_video_system()
{
  return s_running;
}

bool
VideoSystem::is_unavailable(Enum video)
{
  return std::find(s_unavailable.begin(), s_unavailable.end(), video) != s_unavailable.end();
}

VideoSystem::Enum
VideoSystem::get_video_system(const std::string &video)
{
  if (video == "auto")
  {
    return VIDEO_AUTO;
  }
#ifdef HAVE_OPENGL
  else if (video == "opengl" || video == "opengl33" || video == "opengl33core")
  {
    return VIDEO_OPENGL33CORE;
  }
  else if (video == "opengl20")
  {
    return VIDEO_OPENGL20;
  }
#endif
  else if (video == "sdl")
  {
    return VIDEO_SDL;
  }
  else if (video == "null")
  {
    return VIDEO_NULL;
  }
  else
  {
#ifdef HAVE_OPENGL
    throw std::runtime_error("invalid VideoSystem::Enum, valid values are 'auto', 'sdl', 'opengl', 'opengl20' and 'null'");
#else
    throw std::runtime_error("invalid VideoSystem::Enum, valid values are 'auto' and 'sdl'");
#endif
  }
}

std::string
VideoSystem::get_video_string(VideoSystem::Enum video)
{
  switch (video)
  {
    case VIDEO_AUTO:
      return "auto";
    case VIDEO_OPENGL33CORE:
      return "opengl";
    case VIDEO_OPENGL20:
      return "opengl20";
    case VIDEO_SDL:
      return "sdl";
    case VIDEO_NULL:
      return "null";
    default:
      log_fatal << "invalid video system in config" << std::endl;
      assert(false);
      return "auto";
  }
}

std::vector<VideoSystem::Info>
VideoSystem::get_available_video_systems()
{
  std::vector<Info> output;
#ifdef HAVE_OPENGL
  output.push_back({ VIDEO_AUTO, "Auto", "OpenGL 3.3 if the graphics driver has it, then OpenGL 2.0, then SDL" });
  output.push_back({ VIDEO_OPENGL33CORE, "OpenGL 3.3", "Draws every effect, including water and heat haze bending what's behind them" });
  output.push_back({ VIDEO_OPENGL20, "OpenGL 2.0", "For graphics drivers without OpenGL 3.3" });
#else
  output.push_back({ VIDEO_AUTO, "Auto", "SDL, as this build has no OpenGL" });
#endif
  output.push_back({ VIDEO_SDL, "SDL", "Doesn't need OpenGL" });
  return output;
}

/* EOF */
