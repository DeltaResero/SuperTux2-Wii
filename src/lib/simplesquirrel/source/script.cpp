// src/lib/simplesquirrel/source/script.cpp
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2019 Matus Novak <matusnov@gmail.com>

#include "simplesquirrel/object.hpp"
#include "simplesquirrel/script.hpp"
#include <squirrel.h>
#include <forward_list>

namespace ssq {
    Script::Script(HSQUIRRELVM vm_) :Object(vm_) {

    }

    void Script::swap(Script& other) NOEXCEPT {
        Object::swap(other);
    }

    Script::Script(Script&& other) NOEXCEPT :Object(std::forward<Object>(other)) {

    }

    Script& Script::operator = (Script&& other) NOEXCEPT {
        Object::operator = (std::forward<Script>(other));
        return *this;
    }
}
