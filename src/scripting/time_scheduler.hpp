// src/scripting/time_scheduler.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_TIME_SCHEDULER_HPP
#define HEADER_SUPERTUX_SCRIPTING_TIME_SCHEDULER_HPP

#include <vector>

namespace scripting {

/**
 * This class keeps a list of squirrel threads that are scheduled for a certain
 * time. (the typical result of a wait() command in a squirrel script)
 */
class TimeScheduler
{
public:
  TimeScheduler();
  ~TimeScheduler();

  void update(float time);
  void schedule_thread(HSQUIRRELVM vm, float time);

  static TimeScheduler* instance;

private:
  struct ScheduleEntry {
    /// weak reference to the squirrel vm object
    HSQOBJECT thread_ref;
    /// time when the thread should be woken up
    float wakeup_time;

    bool operator<(const ScheduleEntry& other) const
    {
      // we need the smallest value on top
      return wakeup_time > other.wakeup_time;
    }
  };

  typedef std::vector<ScheduleEntry> ScheduleHeap;
  ScheduleHeap schedule;
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_TIME_SCHEDULER_HPP

// EOF
