//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_PROGRAM_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_PROGRAM_HPP

#include <cassert>
#include <memory>
#include <string>

#include "video/gl.hpp"
#include "video/gl/gl_shader.hpp"

class GLProgram final
{
public:
  GLProgram();
  ~GLProgram();

  void bind();

  GLint get_backbuffer_location() const { return m_backbuffer_location; }
  GLint get_fragcoord2uv_location() const { return m_fragcoord2uv_location; }
  GLint get_diffuse_texture_location() const { return m_diffuse_texture_location; }
  GLint get_displacement_texture_location() const { return m_displacement_texture_location; }
  GLint get_framebuffer_texture_location() const { return m_framebuffer_texture_location; }
  GLint get_game_time_location() const { return m_game_time_location; }
  GLint get_modelviewprojection_location() const { return m_modelviewprojection_location; }
  GLint get_animate_location() const { return m_animate_location; }
  GLint get_displacement_animate_location() const { return m_displacement_animate_location; }
  GLint get_position_location() const { return m_position_location; }
  GLint get_texcoord_location() const { return m_texcoord_location; }
  GLint get_diffuse_location() const { return m_diffuse_location; }

private:
  GLint get_attrib_location(const char* name) const;
  GLint get_uniform_location(const char* name) const;
  bool get_link_status() const;
  std::string get_info_log() const;

private:
  GLuint m_program;

  std::unique_ptr<GLShader> m_frag_shader;
  std::unique_ptr<GLShader> m_vert_shader;

  GLint m_backbuffer_location;
  GLint m_fragcoord2uv_location;
  GLint m_diffuse_texture_location;
  GLint m_displacement_texture_location;
  GLint m_framebuffer_texture_location;
  GLint m_game_time_location;
  GLint m_modelviewprojection_location;
  GLint m_animate_location;
  GLint m_displacement_animate_location;
  GLint m_position_location;
  GLint m_texcoord_location;
  GLint m_diffuse_location;

private:
  GLProgram(const GLProgram&) = delete;
  GLProgram& operator=(const GLProgram&) = delete;
};

#endif

/* EOF */
