// src/scripting/thread_queue.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_THREAD_QUEUE_HPP
#define HEADER_SUPERTUX_SCRIPTING_THREAD_QUEUE_HPP

#include <squirrel.h>
#include <vector>

namespace scripting {

/**
 * Keeps a list of SquirrelThreads that wait for a wakeup event
 */
class ThreadQueue
{
public:
  ThreadQueue();
  ~ThreadQueue();

  /// adds a thread (actually a weakref to the thread)
  void add(HSQUIRRELVM vm);
  /// wakes up threads in the list
  void wakeup();

private:
  typedef std::vector<HSQOBJECT> ThreadList;
  ThreadList threads;
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_THREAD_QUEUE_HPP

// EOF
