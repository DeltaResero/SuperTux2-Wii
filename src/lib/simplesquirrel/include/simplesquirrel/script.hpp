// src/lib/simplesquirrel/include/simplesquirrel/script.hpp
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2019 Matus Novak <matusnov@gmail.com>

#pragma once

#include "object.hpp"

namespace ssq {
    /**
    * @brief Squirrel script object
    * @ingroup simplesquirrel
    */
    class SSQ_API Script: public Object {
    public:
        /**
        * @brief Creates empty script object
        */
        Script(HSQUIRRELVM vm_);
        /**
        * @brief Destructor
        */
        virtual ~Script() override = default;
        /**
        * @brief Swaps two objects
        */
        void swap(Script& other) NOEXCEPT;
        /**
        * @brief Deleted copy constructor
        */
        Script(const Script& other) = delete;
        /**
        * @brief Move constructor
        */
        Script(Script&& other) NOEXCEPT;
        /**
        * @brief Deleted copy assignment operator
        */
        Script& operator = (const Script& other) = delete;
        /**
        * @brief Move assignment operator
        */
        Script& operator = (Script&& other) NOEXCEPT;
    };
}
