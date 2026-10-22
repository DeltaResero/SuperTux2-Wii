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

#ifndef HEADER_SUPERTUX_TESTS_ST_ASSERT_HPP
#define HEADER_SUPERTUX_TESTS_ST_ASSERT_HPP

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>

/** Check that an expression is true. */
#define ST_ASSERT(expr) \
  st_assert::check(__FILE__, __LINE__, #expr, static_cast<bool>(expr))

/** Check that two values compare equal, reporting both when they do not. */
#define ST_ASSERT_EQ(expected, actual) \
  st_assert::check_eq(__FILE__, __LINE__, #expected, #actual, (expected), (actual))

/** Check that a statement throws the named exception. */
#define ST_ASSERT_THROW(statement, exception)                                  \
  do {                                                                         \
    bool st_assert_threw = false;                                              \
    try { statement; }                                                         \
    catch (const exception&) { st_assert_threw = true; }                       \
    st_assert::check(__FILE__, __LINE__,                                       \
                     #statement " throws " #exception, st_assert_threw);       \
  } while (false)

namespace st_assert {

template<typename T, typename = void>
struct is_streamable : std::false_type {};

template<typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> :
  std::true_type {};

template<typename T, typename = void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<const T&>())),
                                  decltype(std::end(std::declval<const T&>()))>> :
  std::true_type {};

/** Render a value for a failure message; std::string wins the streamable branch. */
template<typename T>
std::string describe(const T& value)
{
  std::ostringstream out;
  out << std::boolalpha;

  if constexpr (is_streamable<T>::value)
  {
    out << value;
  }
  else if constexpr (is_iterable<T>::value)
  {
    out << '[';
    bool first = true;
    for (const auto& item : value)
    {
      if (!first) out << ", ";
      first = false;
      out << describe(item);
    }
    out << ']';
  }
  else
  {
    out << "<unprintable>";
  }

  return out.str();
}

[[noreturn]]
inline void fail(const char* file, int line, const std::string& message)
{
  std::cerr << file << ":" << line << ": " << message << std::endl;
  std::exit(EXIT_FAILURE);
}

inline void check(const char* file, int line, const char* expr, bool result)
{
  if (!result)
    fail(file, line, std::string("assertion failed: ") + expr);
}

template<typename T, typename U>
void check_eq(const char* file, int line,
              const char* expected_expr, const char* actual_expr,
              const T& expected, const U& actual)
{
  if (!(expected == actual))
    fail(file, line,
         std::string("assertion failed: ") + actual_expr + " == " + expected_expr +
         "\n  expected: " + describe(expected) +
         "\n  actual:   " + describe(actual));
}

} // namespace st_assert

#endif

/* EOF */
