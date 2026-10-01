//  SuperTux
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "object/hurting_platform.hpp"

#include <cmath>

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "object/path_walker.hpp"
#include "object/player.hpp"
#include "supertux/sector.hpp"
#include "util/string_util.hpp"

namespace {

const std::string SAW_SPRITE = "images/objects/sawblade/sawblade.sprite";
const std::string SAW_SOUND = "sounds/sawblade.flac";
const std::string SAW_START_SOUND = "sounds/sawblade_start.flac";

// Where the spin up starts fading out, and the whirr carries straight on from there
const float SAW_START_LENGTH = 3.195f;

} // namespace

HurtingPlatform::HurtingPlatform(const ReaderMapping& reader)
  : Platform(reader, SAW_SPRITE),
    m_saw(StringUtil::has_suffix(m_sprite_name, SAW_SPRITE)),
    m_was_still(false),
    m_start_source(),
    m_sound_source(),
    m_whirr_timer()
{
  set_group(COLGROUP_TOUCHABLE);
  if (m_saw) {
    SoundManager::current()->preload(SAW_START_SOUND);
    SoundManager::current()->preload(SAW_SOUND);
  }
}

HitResponse
HurtingPlatform::collision(GameObject& other, const CollisionHit& )
{
  auto player = dynamic_cast<Player*>(&other);
  if (player) {
    if (player->is_invincible()) {
      return ABORT_MOVE;
    }
    player->kill(false);
  }
  auto badguy = dynamic_cast<BadGuy*>(&other);
  if (badguy) {
    badguy->kill_fall();
  }

  return FORCE_MOVE;
}

void
HurtingPlatform::update(float dt_sec)
{
  Platform::update(dt_sec);

  // Only while it moves, so a saw waiting on a script stays quiet
  if (!m_saw || !get_walker() || !get_walker()->is_running()) {
    m_start_source.reset();
    m_sound_source.reset();
    m_whirr_timer.stop();
    m_was_still = true;
    return;
  }

  // A saw that stood still spins up first, and one running since the level began just whirrs
  if (!m_start_source && !m_sound_source) {
    if (m_was_still) {
      m_start_source = play_saw_sound(SAW_START_SOUND, false);
      m_whirr_timer.start(SAW_START_LENGTH);
    } else {
      m_sound_source = play_saw_sound(SAW_SOUND, true);
    }
  }
  if (m_whirr_timer.check())
    m_sound_source = play_saw_sound(SAW_SOUND, true);
  if (m_start_source && m_sound_source && !m_start_source->playing())
    m_start_source.reset();

  if (m_start_source)
    m_start_source->set_position(get_bbox().get_middle());
  if (m_sound_source)
    m_sound_source->set_position(get_bbox().get_middle());
}

std::unique_ptr<SoundSource>
HurtingPlatform::play_saw_sound(const std::string& file, bool looping) const
{
  auto source = SoundManager::current()->create_sound_source(file);
  source->set_position(get_bbox().get_middle());
  source->set_looping(looping);
  // Turned down by how many saws share the sector, so a crowd of them sounds about as loud as one
  int saws = 0;
  for (const auto& platform : Sector::get().get_objects_by_type<HurtingPlatform>())
    saws += platform.m_saw ? 1 : 0;
  source->set_gain(2.0f / std::sqrt(static_cast<float>(saws)));
  source->set_placed_range();
  source->play();
  return source;
}

void
HurtingPlatform::stop_looping_sounds()
{
  // A spin up that already finished is dropped, or playing it again would start it over
  if (m_start_source && !m_start_source->playing())
    m_start_source.reset();
  if (m_start_source)
    m_start_source->pause();
  if (m_sound_source)
    m_sound_source->pause();
}

void
HurtingPlatform::play_looping_sounds()
{
  if (m_start_source)
    m_start_source->play();
  if (m_sound_source)
    m_sound_source->play();
}

/* EOF */
