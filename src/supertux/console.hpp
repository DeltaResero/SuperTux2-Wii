// src/supertux/console.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Console
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_CONSOLE_HPP
#define HEADER_SUPERTUX_SUPERTUX_CONSOLE_HPP

#include <deque>
#include <memory>
#include <squirrel.h>
#include <sstream>
#include <vector>

#include "util/currenton.hpp"
#include "video/font_ptr.hpp"
#include "video/surface_ptr.hpp"

#ifndef DISABLE_CONSOLE
class Console;
#else
class Console; // Forward declaration might still be needed by pointers, though we guard usage
#endif
class ConsoleStreamBuffer;
class DrawingContext;

class ConsoleBuffer : public Currenton<ConsoleBuffer>
{
public:
  static std::ostream output; /**< stream of characters to output to the console. Do not forget to send std::endl or to flush the stream. */
  static ConsoleStreamBuffer s_outputBuffer; /**< stream buffer used by output stream */

public:
#ifndef DISABLE_CONSOLE
  std::deque<std::string> m_lines; /**< backbuffer of lines sent to the console. New lines get added to front. */
  Console* m_console;
#endif

public:
  ConsoleBuffer();

  void addLines(const std::string& s); /**< display a string of (potentially) multiple lines in the console */
  void addLine(const std::string& s); /**< display a line in the console */

  void flush(ConsoleStreamBuffer& buffer); /**< act upon changes in a ConsoleStreamBuffer */

#ifndef DISABLE_CONSOLE
  void set_console(Console* console);
#endif

private:
  ConsoleBuffer(const ConsoleBuffer&) = delete;
  ConsoleBuffer& operator=(const ConsoleBuffer&) = delete;
};

#ifndef DISABLE_CONSOLE
class Console : public Currenton<Console>
{
public:
  Console(ConsoleBuffer& buffer);
  ~Console();

  void on_buffer_change(int line_count);

  void input(char c); /**< add character to inputBuffer */
  void backspace(); /**< delete character left of inputBufferPosition */
  void eraseChar(); /**< delete character at inputBufferPosition */
  void enter(); /**< process and clear input stream */
  void scroll(int offset); /**< scroll console text up or down by @c offset lines */
  void autocomplete(); /**< autocomplete current command */
  void show_history(int offset); /**< move @c offset lines forward through history; Negative offset moves backward */
  void move_cursor(int offset); /**< move the cursor @c offset chars to the right; Negative offset moves backward; 0xFFFF moves to the end */

  void draw(DrawingContext& context) const; /**< draw the console in a DrawingContext */
  void update(float elapsed_time);

  void show(); /**< display the console */
  void open(); /**< open the console for viewing for 6 seconds */
  void hide(); /**< hide the console */
  void toggle(); /**< display the console if hidden, hide otherwise */

  bool hasFocus() const; /**< true if characters should be sent to the console instead of their normal target */
  FontPtr get_font() const {
    return m_font;
  }

private:
  ConsoleBuffer& m_buffer;

  std::string m_inputBuffer; /**< string used for keyboard input */
  int m_inputBufferPosition; /**< position in inputBuffer before which to append new characters */

  std::deque<std::string> m_history; /**< command history. New lines get added to back. */
  std::deque<std::string>::iterator m_history_position; /**< item of command history that is currently displayed */

  SurfacePtr m_background; /**< console background image */
  SurfacePtr m_background2; /**< second, moving console background image */

  HSQUIRRELVM m_vm; /**< squirrel thread for the console (with custom roottable) */
  HSQOBJECT m_vm_object;

  int m_backgroundOffset; /**< current offset of scrolling background image */
  float m_height; /**< height of the console in px */
  float m_alpha;
  int m_offset; /**< decrease to scroll text up */
  bool m_focused; /**< true if console has input focus */
  FontPtr m_font;

  float m_stayOpen;

  void parse(const std::string& s); /**< react to a given command */

  /** ready a virtual machine instance, creating a new thread and loading default .nut files if needed */
  void ready_vm();

  /** execute squirrel script and output result */
  void execute_script(const std::string& s);

  bool consoleCommand(const std::string& command, const std::vector<std::string>& arguments); /**< process internal command; return false if command was unknown, true otherwise */

private:
  Console(const Console&);
  Console & operator=(const Console&);
};
#endif // DISABLE_CONSOLE

class ConsoleStreamBuffer : public std::stringbuf
{
public:
  int sync()
  {
    int result = std::stringbuf::sync();
    if(ConsoleBuffer::current())
      ConsoleBuffer::current()->flush(*this);
    return result;
  }
};

#endif // HEADER_SUPERTUX_SUPERTUX_CONSOLE_HPP

// EOF
