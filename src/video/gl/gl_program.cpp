//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/gl/gl_program.hpp"

#include <sstream>
#include <stdexcept>
#include <vector>

#include "util/log.hpp"
#include "video/glutil.hpp"

GLProgram::GLProgram() :
  m_program(glCreateProgram()),
  m_frag_shader(),
  m_vert_shader(),
  m_backbuffer_location(-1),
  m_fragcoord2uv_location(-1),
  m_diffuse_texture_location(-1),
  m_displacement_texture_location(-1),
  m_framebuffer_texture_location(-1),
  m_game_time_location(-1),
  m_modelviewprojection_location(-1),
  m_animate_location(-1),
  m_displacement_animate_location(-1),
  m_position_location(-1),
  m_texcoord_location(-1),
  m_diffuse_location(-1)
{
  assert_gl();

#if defined(USE_OPENGLES2)
  m_frag_shader = GLShader::from_file(GL_FRAGMENT_SHADER, "shader/shader100.frag");
  m_vert_shader = GLShader::from_file(GL_VERTEX_SHADER, "shader/shader100.vert");
#else
  m_frag_shader = GLShader::from_file(GL_FRAGMENT_SHADER, "shader/shader330.frag");
  m_vert_shader = GLShader::from_file(GL_VERTEX_SHADER, "shader/shader330.vert");
#endif
  assert_gl();

  glAttachShader(m_program, m_frag_shader->get_handle());
  glAttachShader(m_program, m_vert_shader->get_handle());

  glLinkProgram(m_program);

  if (!get_link_status())
  {
    std::ostringstream out;
    out << "link failure:\n" << get_info_log() << std::endl;
    throw std::runtime_error(out.str());
  }

  // A linked program's locations never change
  m_backbuffer_location = get_uniform_location("backbuffer");
  m_fragcoord2uv_location = get_uniform_location("fragcoord2uv");
  m_diffuse_texture_location = get_uniform_location("diffuse_texture");
  m_displacement_texture_location = get_uniform_location("displacement_texture");
  m_framebuffer_texture_location = get_uniform_location("framebuffer_texture");
  m_game_time_location = get_uniform_location("game_time");
  m_modelviewprojection_location = get_uniform_location("modelviewprojection");
  m_animate_location = get_uniform_location("animate");
  m_displacement_animate_location = get_uniform_location("displacement_animate");
  m_position_location = get_attrib_location("position");
  m_texcoord_location = get_attrib_location("texcoord");
  m_diffuse_location = get_attrib_location("diffuse");

  assert_gl();
}

GLProgram::~GLProgram()
{
  m_vert_shader.reset();
  m_frag_shader.reset();
  glDeleteProgram(m_program);
}

void
GLProgram::bind()
{
  assert_gl();

  glUseProgram(m_program);

  assert_gl();
}

GLint
GLProgram::get_attrib_location(const char* name) const
{
  assert_gl();

  GLint loc = glGetAttribLocation(m_program, name);
  if (loc == -1)
  {
    log_debug << "GLProgram::get_attrib_location(\"" << name << "\") failed" << std::endl;
  }

  assert_gl();

  return loc;
}

GLint
GLProgram::get_uniform_location(const char* name) const
{
  assert_gl();

  GLint loc = glGetUniformLocation(m_program, name);
  if (loc == -1)
  {
    log_debug << "GLProgram::get_uniform_location(\"" << name << "\") failed" << std::endl;
  }

  assert_gl();

  return loc;
}

bool
GLProgram::get_link_status() const
{
  assert_gl();

  GLint link_status;
  glGetProgramiv(m_program, GL_LINK_STATUS, &link_status);

  assert_gl();

  return link_status != 0;
}

std::string
GLProgram::get_info_log() const
{
  assert_gl();

  GLint length;
  glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

  assert_gl();

  if (length == 0)
  {
    return {};
  }
  else
  {
    GLsizei out_length;
    std::vector<char> str(length);
    glGetProgramInfoLog(m_program, static_cast<GLsizei>(str.size()), &out_length, str.data());
    assert_gl();
    return std::string(str.begin(), str.end());
  }
}

/* EOF */
