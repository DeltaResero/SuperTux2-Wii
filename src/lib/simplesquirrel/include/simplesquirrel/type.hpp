// src/lib/simplesquirrel/include/simplesquirrel/type.hpp
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2019 Matus Novak <matusnov@gmail.com>

#pragma once

#include <squirrel.h>

#include <cassert>
#include <utility>

#if defined(SSQ_DLL) && defined(_MSC_VER)
    #ifdef SSQ_EXPORTS
		#define SSQ_API __declspec(dllexport)
    #else
		#define SSQ_API __declspec(dllimport)
    #endif
#else
    #define SSQ_API
#endif

namespace ssq {
    /* a cast the debug build proves the value survives */
    template<typename To, typename From>
    inline To narrow(From v) {
        assert(std::in_range<To>(v));
        return static_cast<To>(v);
    }
    /**
     * @ingroup simplesquirrel
     */
    enum class Type {
        NULLPTR = OT_NULL,
        INTEGER = OT_INTEGER,
        FLOAT = OT_FLOAT,
        BOOL = OT_BOOL,
        STRING = OT_STRING,
        TABLE = OT_TABLE,
        ARRAY = OT_ARRAY,
        USERDATA = OT_USERDATA,
        CLOSURE = OT_CLOSURE,
        NATIVECLOSURE = OT_NATIVECLOSURE,
        GENERATOR = OT_GENERATOR,
        USERPOINTER = OT_USERPOINTER,
        THREAD = OT_THREAD,
        FUNCPROTO = OT_FUNCPROTO,
        CLASS = OT_CLASS,
        INSTANCE = OT_INSTANCE,
        WEAKREF = OT_WEAKREF,
        OUTER = OT_OUTER
    };
    /**
     * @ingroup simplesquirrel
     */
    SSQ_API const char* typeToStr(Type type);
}
