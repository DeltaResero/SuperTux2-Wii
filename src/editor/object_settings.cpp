// src/editor/object_settings.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "editor/object_settings.hpp"
#include "video/color.hpp"

ObjectSettings::ObjectSettings(const std::string& name_) :
  name(name_),
  options()
{
  options.clear();
}

ObjectSettings::~ObjectSettings() {

}

void ObjectSettings::copy_from(ObjectSettings* other) {
  auto it1 = options.begin();
  auto it2 = other->options.begin();
  while (it1 != options.end() && it2 != other->options.end()) {
    auto oo1 = &*it1;
    auto oo2 = &*it2;

    switch (oo1->type) {
      case MN_TEXTFIELD:
      case MN_SCRIPT:
      case MN_FILE:
        assert(oo1->type == oo2->type);
        *((std::string*)oo1->option) = *((std::string*)oo2->option);
        break;
      case MN_NUMFIELD:
        assert(oo1->type == oo2->type);
        *((float*)oo1->option) = *((float*)oo2->option);
        break;
      case MN_INTFIELD:
      case MN_STRINGSELECT:
        assert(oo1->type == oo2->type);
        *((int*)oo1->option) = *((int*)oo2->option);
        break;
      case MN_TOGGLE:
        assert(oo1->type == oo2->type);
        *((bool*)oo1->option) = *((bool*)oo2->option);
        break;
      case MN_BADGUYSELECT:
        assert(oo1->type == oo2->type);
        *((std::vector<std::string>*)oo1->option) = *((std::vector<std::string>*)oo2->option);
        break;
      case MN_COLOR:
        assert(oo1->type == oo2->type);
        *((Color*)oo1->option) = *((Color*)oo2->option);
        break;
      default:
        //Do not assert here!
        break;
    }

    it1++;
    it2++;
  }
}

// EOF
