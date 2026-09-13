// src/lib/simplesquirrel/include/simplesquirrel/exposable_class.hpp
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2019 Matus Novak <matusnov@gmail.com>

#pragma once

namespace ssq {
    /**
    * @brief Any exposed classes must inherit this interface
    * @ingroup simplesquirrel
    */
    class ExposableClass {
    public:
        /**
        * @brief Constructs an instance
        */
        ExposableClass() {}
        /**
        * @brief Virtual destructor
        */
        virtual ~ExposableClass() {}
    };
}
