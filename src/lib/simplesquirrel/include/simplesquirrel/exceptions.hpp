// src/lib/simplesquirrel/include/simplesquirrel/exceptions.hpp
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2019 Matus Novak <matusnov@gmail.com>

#pragma once

#include <string>
#include <sstream>
#include <squirrel.h>

namespace ssq {
    /**
    * @brief Raw exception
    * @ingroup simplesquirrel
    */
    class Exception: public std::exception {
    public:
        HSQUIRRELVM vm;

        Exception(HSQUIRRELVM v) : vm(v), message() {}
        Exception(HSQUIRRELVM v, const std::string& msg) : vm(v), message() {
            generate_message(msg);
        }

        /* vm is borrowed, so copying the handle is what we want */
        Exception(const Exception &) = default;
        Exception &operator=(const Exception &) = default;
        Exception(Exception &&) = default;
        Exception &operator=(Exception &&) = default;

        virtual const char* what() const throw() override {
            return message.c_str();
        }

    protected:
        void generate_message(const std::string& msg);

    private:
        std::string message;
    };
    /**
    * @brief Not Found exception thrown if object with a given name does not exist
    * @ingroup simplesquirrel
    */
    class NotFoundException: public Exception {
    public:
        NotFoundException(HSQUIRRELVM v, const std::string& msg) : Exception(v) {
            std::stringstream ss;
            ss << "Not found: " << msg;
            generate_message(ss.str());
        }
    };
    /**
    * @brief Compile exception thrown during compilation
    * @ingroup simplesquirrel
    */
    class CompileException: public Exception {
    public:
        CompileException(HSQUIRRELVM v, const std::string& msg) : Exception(v, msg) {}
        CompileException(HSQUIRRELVM v, const std::string& msg, const char* source, int line, int column) : Exception(v) {
            std::stringstream ss;
            ss << "Compile error at " << source << ":" << line << ":" << column << " " << msg;
            generate_message(ss.str());
        }
    };
    /**
    * @brief Type exception thrown if casting between squirrel and C++ objects failed
    * @ingroup simplesquirrel
    */
    class TypeException: public Exception {
    public:
        TypeException(const std::string& msg, const char* expected, const char* got) : Exception(nullptr) {
            std::stringstream ss;
            ss << "Type error " << msg << " expected: " << expected << " got: " << got;
            generate_message(ss.str());
        }
    };
    /**
    * @brief Runtime exception thrown if something went wrong during execution
    * @ingroup simplesquirrel
    */
    class RuntimeException: public Exception {
    public:
        RuntimeException(HSQUIRRELVM v, const std::string& msg) : Exception(v, msg) {}
        RuntimeException(HSQUIRRELVM v, const std::string& msg, const char* source, const char* func, int line) : Exception(v) {
            std::stringstream ss;
            ss << "Runtime error at (" << func << ") " << source << ":" << line << ": " << msg;
            generate_message(ss.str());
        }
    };
}
