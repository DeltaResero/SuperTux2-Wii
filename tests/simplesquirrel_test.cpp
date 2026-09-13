// tests/simplesquirrel_test.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include "st_assert.hpp"

#include <simplesquirrel/simplesquirrel.hpp>

#include <string>

namespace {

/** One call for each ssq::narrow<> site: a stack size, a source length, a
    parameter count and an array length. */
void test_conversions()
{
  ssq::VM vm(1024, ssq::Libs::ALL);

  ssq::Script script = vm.compileSource(
    "answer <- 42;                                   \n"
    "function twice(n) { return n * 2; }             \n"
    "function join(a, b) { return a + \"/\" + b; }   \n"
    "function count(s) { return s.len(); }           \n");
  vm.run(script);

  ST_ASSERT_EQ(42, vm.find("answer").toInt());

  ssq::Function twice = vm.findFunc("twice");
  ST_ASSERT_EQ(14, vm.callFunc(twice, vm, 7).toInt());

  // a std::string out through push and back in through pop
  ssq::Function join = vm.findFunc("join");
  const std::string got = vm.callFunc(join, vm, std::string("a"), std::string("b")).toString();
  ST_ASSERT_EQ(std::string("a/b"), got);

  // the length of a string built on the C++ side, measured on the script side
  ssq::Function count = vm.findFunc("count");
  ST_ASSERT_EQ(5, vm.callFunc(count, vm, std::string("hello")).toInt());

  ssq::Array arr(vm.getHandle(), 4);
  ST_ASSERT_EQ(static_cast<size_t>(4), arr.size());
}

/** The remaining narrow<strlen(name)> sites are all lookups by name. */
void test_lookup_by_name()
{
  ssq::VM vm(1024, ssq::Libs::ALL);

  ssq::Script script = vm.compileSource(
    "nested <- { inner = { value = 3 } };            \n"
    "class Counter {                                 \n"
    "  n = 0;                                        \n"
    "  constructor() { n = 0; }                      \n"
    "  function bump() { n = n + 1; return n; }      \n"
    "}                                               \n");
  vm.run(script);

  ssq::Table nested = vm.findTable("nested");
  ST_ASSERT_EQ(3, nested.findTable("inner").find("value").toInt());

  ssq::Class counter = vm.findClass("Counter");
  ssq::Instance inst = vm.newInstance(counter);
  ssq::Function bump = counter.findFunc("bump");
  ST_ASSERT_EQ(1, vm.callFunc(bump, inst).toInt());
  ST_ASSERT_EQ(2, vm.callFunc(bump, inst).toInt());
}

/** The error callbacks are the one path that formats a message of its own. */
void test_errors()
{
  ssq::VM vm(1024, ssq::Libs::ALL);
  vm.setStdErrorFunc();

  ST_ASSERT_THROW(vm.compileSource("this is not squirrel"), ssq::CompileException);

  ssq::Script bad = vm.compileSource("function boom() { throw \"kaboom\"; }");
  vm.run(bad);
  ssq::Function boom = vm.findFunc("boom");
  ST_ASSERT_THROW(vm.callFunc(boom, vm), ssq::RuntimeException);

  // a wrong number of arguments is caught before the VM is entered
  ssq::Script two = vm.compileSource("function two(a, b) { return a + b; }");
  vm.run(two);
  ST_ASSERT_THROW(vm.callFunc(vm.findFunc("two"), vm, 1), ssq::RuntimeException);

  ST_ASSERT_THROW(vm.findFunc("nosuchfunc"), ssq::NotFoundException);
}

/** what() wraps the message, so a copy is checked by looking inside it. */
bool mentions(const char* what, const std::string& msg)
{
  return std::string(what).find(msg) != std::string::npos;
}

/** ssq::Exception holds a std::string and a borrowed VM handle. */
void test_exception_copies()
{
  ssq::VM vm(1024, ssq::Libs::NONE);

  const ssq::RuntimeException original(vm.getHandle(), "first");
  ST_ASSERT(mentions(original.what(), "first"));

  const ssq::RuntimeException copied(original);
  ST_ASSERT(mentions(copied.what(), "first"));

  ssq::RuntimeException assigned(vm.getHandle(), "other");
  assigned = original;
  ST_ASSERT(mentions(assigned.what(), "first"));

  ssq::RuntimeException source(vm.getHandle(), "second");
  const ssq::RuntimeException moved(std::move(source));
  ST_ASSERT(mentions(moved.what(), "second"));

  // caught as the base class, the message still has to survive the slice
  try
  {
    throw ssq::RuntimeException(vm.getHandle(), "third");
  }
  catch (const ssq::Exception& e)
  {
    ST_ASSERT(mentions(e.what(), "third"));
  }
}

} // namespace

int main()
{
  test_conversions();
  test_lookup_by_name();
  test_errors();
  test_exception_copies();

  return 0;
}

/* EOF */
