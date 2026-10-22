//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "util/dynamic_scoped_ref.hpp"

DynamicScopedRef<const int> d_value;

void check_function(int expected)
{
  ST_ASSERT_EQ(*d_value.get(), expected);
}

int main()
{
  int v1 = 1;
  int v2 = 2;
  int v3 = 3;

  {
    auto guard1 = d_value.bind(v1);

    ST_ASSERT(d_value);
    ST_ASSERT_EQ(*d_value, 1);
    check_function(1);
    {
      auto guard2 = d_value.bind(v2);

      ST_ASSERT(d_value);
      ST_ASSERT_EQ(*d_value, 2);
      check_function(2);
      {
        auto guard3 = d_value.bind(v3);
        ST_ASSERT_EQ(*d_value, 3);
        check_function(3);
      }
      ST_ASSERT(d_value);
      check_function(2);
      ST_ASSERT_EQ(*d_value, 2);
    }
    ST_ASSERT(d_value);
    ST_ASSERT_EQ(*d_value, 1);
    check_function(1);
  }

  ST_ASSERT(!d_value);

  return 0;
}

/* EOF */
