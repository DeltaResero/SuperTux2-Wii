// src/supertux/game_object.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/game_object.hpp"
#include "supertux/object_remove_listener.hpp"

GameObject::GameObject() :
  wants_to_die(false),
  remove_listeners(NULL),
  name()
{
}

GameObject::GameObject(const GameObject& rhs) :
  wants_to_die(rhs.wants_to_die),
  remove_listeners(NULL),
  name(rhs.name)
{
}

GameObject::~GameObject()
{
  // call remove listeners (and remove them from the list)
  auto entry = remove_listeners;
  while(entry != NULL) {
    auto next = entry->next;
    entry->listener->object_removed(this);
    delete entry;
    entry = next;
  }
}

void
GameObject::add_remove_listener(ObjectRemoveListener* listener)
{
  auto entry = new RemoveListenerListEntry();
  entry->next = remove_listeners;
  entry->listener = listener;
  remove_listeners = entry;
}

void
GameObject::del_remove_listener(ObjectRemoveListener* listener)
{
  auto entry = remove_listeners;
  if (entry->listener == listener) {
    remove_listeners = entry->next;
    delete entry;
    return;
  }
  auto next = entry->next;
  while(next != NULL) {
    if (next->listener == listener) {
      entry->next = next->next;
      delete next;
      break;
    }
    entry = next;
    next = next->next;
  }
}

void
GameObject::save(Writer& writer) {
  if(name != "") {
    writer.write("name", name, false);
  }
}

// EOF
