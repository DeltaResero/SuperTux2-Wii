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

#include "math/random.hpp"

namespace {

void rand1i_test()
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    int v = random.rand(10);
    ST_ASSERT(0 <= v);
    ST_ASSERT(v < 10);
  }
}

void rand2i_test()
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    int v = random.rand(10, 20);
    ST_ASSERT(10 <= v);
    ST_ASSERT(v < 20);
  }
}

void rand1f_test()
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    float v = random.randf(10);
    ST_ASSERT(0.0f <= v);
    ST_ASSERT(v < 10.0f);
  }
}

void rand2f_test()
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    float v = random.randf(10.0f, 20.0f);
    ST_ASSERT(10.0f <= v);
    ST_ASSERT(v < 20.0f);
  }
}

void rand_determinism_test()
{
  Random random;

  random.seed(0);
  std::vector<int> run1;
  run1.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run1.push_back(random.rand());
  }

  random.seed(0);
  std::vector<int> run2;
  run2.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run2.push_back(random.rand());
  }

  ST_ASSERT_EQ(run1, run2);
}

void randf_determinism_test()
{
  Random random;

  random.seed(0);
  std::vector<float> run1;
  run1.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run1.push_back(random.randf(1.0f));
  }

  random.seed(0);
  std::vector<float> run2;
  run2.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run2.push_back(random.randf(1.0f));
  }

  ST_ASSERT_EQ(run1, run2);
}

} // namespace

int main()
{
  rand1i_test();
  rand2i_test();
  rand1f_test();
  rand2f_test();
  rand_determinism_test();
  randf_determinism_test();

  return 0;
}

/* EOF */
