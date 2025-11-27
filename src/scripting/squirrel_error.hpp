// src/scripting/squirrel_error.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_SQUIRREL_ERROR_HPP
#define HEADER_SUPERTUX_SCRIPTING_SQUIRREL_ERROR_HPP

#include <squirrel.h>
#include <stdexcept>
#include <string>

namespace scripting {

/** Exception class for squirrel errors, it takes a squirrelvm and uses
 * sq_geterror() to retrieve additional information about the last error that
 * occurred and creates a readable message from that.
 */
class SquirrelError : public std::exception
{
public:
  SquirrelError(HSQUIRRELVM v, const std::string& message) throw();
  virtual ~SquirrelError() throw();

  const char* what() const throw();
private:
  std::string message;
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_SQUIRREL_ERROR_HPP

// EOF
