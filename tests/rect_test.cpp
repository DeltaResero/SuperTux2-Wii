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

#include "math/rect.hpp"

namespace {

void contains_point_test()
{
  ST_ASSERT(Rect(100, 100, 200, 200).contains(150, 150));
  ST_ASSERT(!Rect(100, 100, 200, 200).contains(250, 150));
}

void contains_rect_test()
{
  ST_ASSERT(Rect(100, 100, 200, 200).contains(Rect(150, 150, 190, 190)));
  ST_ASSERT(Rect(100, 100, 200, 200).contains(Rect(100, 100, 200, 200)));
  ST_ASSERT(!Rect(100, 100, 200, 200).contains(Rect(150, 150, 250, 250)));
}

void moved_test()
{
  ST_ASSERT_EQ(Rect(0, 0, 100, 300).moved(16, 32), Rect(16, 32, 116, 332));
  ST_ASSERT_EQ(Rect(0, 0, 100, 300).moved(-16, -32), Rect(-16, -32, 84, 268));
}

void normalized_test()
{
  ST_ASSERT_EQ(Rect(0, 0, 100, 300).normalized(), Rect(0, 0, 100, 300));
  ST_ASSERT_EQ(Rect(100, 300, 0, 0).normalized(), Rect(0, 0, 100, 300));
}

void valid_test()
{
  ST_ASSERT(Rect(0, 0, 100, 300).valid());
  ST_ASSERT(!Rect(100, 300, 0, 0).valid());
  ST_ASSERT(Rect(0, 0, 0, 0).valid());
}

void empty_test()
{
  ST_ASSERT(!Rect(0, 0, 100, 300).empty());
  ST_ASSERT(Rect(100, 300, 0, 0).empty());
  ST_ASSERT(Rect(0, 0, 0, 0).empty());
}

void size_test()
{
  ST_ASSERT_EQ(Rect(50, 50, 100, 300).get_width(), 50);
  ST_ASSERT_EQ(Rect(50, 50, 100, 300).get_height(), 250);
}

void from_center_test()
{
  ST_ASSERT_EQ(Rect::from_center(16, 16, 32, 32), Rect(0, 0, 32, 32));
}

void SDL_test()
{
  // SDL
  const SDL_Rect sdl_rect_result = Rect(50, 50, 100, 100).to_sdl();
  const SDL_Rect sdl_rect_expected{50, 50, 50, 50};
  ST_ASSERT(SDL_RectEquals(&sdl_rect_result, &sdl_rect_expected));
  ST_ASSERT_EQ(Rect(SDL_Rect{50, 50, 50, 50}), Rect(50, 50, 100, 100));
}

} // namespace

int main()
{
  contains_point_test();
  contains_rect_test();
  moved_test();
  normalized_test();
  valid_test();
  empty_test();
  size_test();
  from_center_test();
  SDL_test();

  return 0;
}

/* EOF */
