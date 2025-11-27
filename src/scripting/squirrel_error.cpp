// src/scripting/squirrel_error.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "scripting/squirrel_error.hpp"
#include <sstream>

namespace scripting {

SquirrelError::SquirrelError(HSQUIRRELVM v, const std::string& message_) throw() :
  message()
{
  std::ostringstream msg;
  msg << "Squirrel error: " << message_ << " (";
  const char* lasterr;
  sq_getlasterror(v);
  if(sq_gettype(v, -1) != OT_STRING)
  {
    lasterr = "no error info";
  }
  else
  {
    sq_getstring(v, -1, &lasterr);
  }
  msg << lasterr << ")";
  sq_pop(v, 1);
  this->message = msg.str();
}

SquirrelError::~SquirrelError() throw()
{}

const char*
SquirrelError::what() const throw()
{
  return message.c_str();
}

}

// EOF
