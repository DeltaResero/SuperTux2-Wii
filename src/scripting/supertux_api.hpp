//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#ifndef HEADER_SUPERTUX_SCRIPTING_SUPERTUX_API_HPP
#define HEADER_SUPERTUX_SCRIPTING_SUPERTUX_API_HPP

#include <sstream>
#include <typeinfo>

#include <simplesquirrel/exposable_class.hpp>
#include <simplesquirrel/vm.hpp>
#include <squirrel.h>

#include "squirrel/squirrel_error.hpp"

namespace scripting {

/** Puts every class, function and constant a script can reach into the root
    table, which has to be on top of the VM's stack. */
void register_supertux_api(HSQUIRRELVM v);

/** Deletes an object squirrel was given to own. */
SQInteger release_hook(SQUserPointer ptr, SQInteger size);

/** Makes a squirrel instance of a registered class standing for the object.
    The class comes from the registry simplesquirrel fills in as it binds. */
template<typename T>
void create_squirrel_instance(HSQUIRRELVM v, T* object, bool setup_releasehook = false)
{
  sq_pushobject(v, ssq::VM::getClassObj(typeid(T*).hash_code()));

  // simplesquirrel reads every instance back as an ExposableClass pointer
  if (SQ_FAILED(sq_createinstance(v, -1)) ||
      SQ_FAILED(sq_setinstanceup(v, -1, static_cast<ssq::ExposableClass*>(object)))) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object of type '" << typeid(T).name() << "'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2);

  if (setup_releasehook) {
    sq_setreleasehook(v, -1, &release_hook);
  }
}

} // namespace scripting

#endif

/* EOF */
