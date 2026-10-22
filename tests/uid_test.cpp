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

#include <unordered_set>

#include "util/uid_generator.hpp"

namespace {

void null_test()
{
  UIDGenerator generator;
  UID null_uid;
  UID uid = generator.next();

  ST_ASSERT(!null_uid);
  ST_ASSERT(uid);
  ST_ASSERT(uid != null_uid);
}

void magic_test()
{
  UIDGenerator generator1;
  UIDGenerator generator2;

  UID uid1 = generator1.next();
  UID uid2 = generator2.next();

  ST_ASSERT(uid1 != uid2);
}

void copy_test()
{
  UIDGenerator generator;
  UID uid = generator.next();
  UID other = uid;

  ST_ASSERT_EQ(uid, other);
}

void unique_test()
{
  if ((false)) {
    UIDGenerator generator;
    std::unordered_set<UID> uids;

    for(int i = 0; i < 0xffffff; ++i)
    {
      UID uid = generator.next();
      ST_ASSERT(uids.find(uid) == uids.end());
      uids.insert(uid);
    }
  }
}

} // namespace

int main()
{
  null_test();
  magic_test();
  copy_test();
  unique_test();

  return 0;
}

/* EOF */
