//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "audio/sound_manager.hpp"

#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <set>

#include "audio/dummy_sound_source.hpp"
#include "audio/openal_device.hpp"
#include "audio/sound_source.hpp"
#include "math/util.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"

namespace {

// Within this of Tux a sound plays at full volume in both ears
const float BESIDE_TUX = 100.0f;

// About two screen widths, and BadGuy's X_OFFSCREEN_DISTANCE, so a sound is gone by the time its enemy stops running
const float PLACED_RANGE = 1280.0f;

// Vanilla's listener setback, reference distances and OpenAL Soft's pan, 0.5 +- 0.5 sin(t) + FRONT cos(t) per ear
const float VANILLA_SETBACK = 300.0f;
const float VANILLA_PLACED_REFERENCE = 128.0f;
const float VANILLA_CLOSE_REFERENCE = 32.0f;
const float VANILLA_PAN_FRONT = 0.0956f;
const float VANILLA_PAN_STRETCH = 1.5f;

// These were stereo files, which vanilla's OpenAL never placed and so played at full volume
const float FULL_LEVEL = 1.0f;

// The far ear stays within 10 dB of the near one, as a sound in one ear alone is uncomfortable to hear
const float FAR_EAR_FLOOR = 0.316f;

bool was_stereo(const std::string& filename)
{
  static const std::set<std::string> names = {
    "cracking", "crystallo-shardhit", "crystallo-shatter", "fall", "fire", "firecracker",
    "grunts", "icecrash", "sizzle", "squish", "stomp", "trampoline"
  };
  const std::string name = FileSystem::basename(filename);
  return names.count(name.substr(0, name.rfind('.'))) > 0;
}

// Vanilla's pan for a mono sound dx to the side of Tux
void vanilla_pan(float dx, float& near_ear, float& far_ear)
{
  const float turn = std::min(VANILLA_PAN_STRETCH * std::atan2(std::abs(dx), VANILLA_SETBACK), math::PI_2);
  near_ear = 0.5f + 0.5f * std::sin(turn) + VANILLA_PAN_FRONT * std::cos(turn);
  far_ear = 0.5f - 0.5f * std::sin(turn) + VANILLA_PAN_FRONT * std::cos(turn);
}

} // namespace

SoundManager::SoundManager() :
  m_device(std::make_unique<OpenALDevice>()),
  m_sound_enabled(false),
  m_sound_volume(0),
  m_sources(),
  m_update_list(),
  m_music_enabled(false),
  m_music_volume(0),
  m_current_music(),
  m_player_position()
{
  if (m_device->is_open()) {
    m_sound_enabled = true;
    m_music_enabled = true;

    set_listener_orientation(Vector(0.0f, 0.0f), Vector(0.0f, -1.0f));
  }
}

SoundManager::~SoundManager()
{
  // The device owns the buffers the sources play, so they go first
  m_sources.clear();
  m_device.reset();
}

bool
SoundManager::is_audio_enabled() const
{
  return m_device && m_device->is_open();
}

std::unique_ptr<SoundSource>
SoundManager::intern_create_sound_source(const std::string& filename)
{
  assert(m_sound_enabled);

  auto source = m_device->create_source(filename, was_stereo(filename));
  source->set_volume(static_cast<float>(m_sound_volume) / 100.0f);
  return source;
}

std::unique_ptr<SoundSource>
SoundManager::create_sound_source(const std::string& filename)
{
  if (!m_sound_enabled)
    return create_dummy_sound_source();

  try {
    return intern_create_sound_source(filename);
  } catch(std::exception &e) {
    log_warning << "Couldn't create audio source: " << e.what() << std::endl;
    return create_dummy_sound_source();
  }
}

void
SoundManager::preload(const std::string& filename)
{
  if (!m_sound_enabled)
    return;

  m_device->preload(filename);
}

void
SoundManager::play(const std::string& filename, const std::optional<Vector>& pos,
  const float gain)
{
  if (!m_sound_enabled)
    return;

  // Test gain for invalid values; it must not exceed 1 because in the end
  // the value is set to min(sound_gain * sound_volume, 1)
  assert(gain >= 0.0f && gain <= 1.0f);

  try {
    std::unique_ptr<SoundSource> source(intern_create_sound_source(filename));
    source->set_gain(gain);

    if (!pos) {
      source->set_relative(true);
    } else {
      source->set_placed_range();
      source->set_position(*pos);
    }
    source->play();
    m_sources.push_back(std::move(source));
  } catch(std::exception& e) {
    log_warning << "Couldn't play sound " << filename << ": " << e.what() << std::endl;
  }
}

void
SoundManager::manage_source(std::unique_ptr<SoundSource> source)
{
  assert(source);
  // A sound that couldn't be made is a dummy, which never finishes playing
  if (!is_dummy_sound_source(*source))
    m_sources.push_back(std::move(source));
}

void
SoundManager::register_for_update(SoundSource* source)
{
  if (source)
  {
    m_update_list.push_back(source);
  }
}

void
SoundManager::remove_from_update(SoundSource* source)
{
  if (source)
  {
    auto it = m_update_list.begin();
    while (it != m_update_list.end()) {
      if (*it == source) {
        it = m_update_list.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void
SoundManager::enable_sound(bool enable)
{
  if (!is_audio_enabled())
    return;

  m_sound_enabled = enable;
}

void
SoundManager::enable_music(bool enable)
{
  if (!is_audio_enabled())
    return;

  m_music_enabled = enable;
  if (m_music_enabled) {
    play_music(m_current_music);
  } else {
    m_device->stop_music(0);
  }
}

void
SoundManager::stop_music(float fadetime)
{
  m_device->stop_music(fadetime);
  m_current_music = "";
}

void
SoundManager::set_music_volume(int volume)
{
  m_music_volume = volume;
  m_device->set_music_volume(static_cast<float>(volume) / 100.0f);
}

void
SoundManager::play_music(const std::string& filename, float fadetime)
{
  if (filename == m_current_music && m_device->has_music())
  {
    m_device->keep_music_playing();
    return;
  }
  m_current_music = filename;
  if (!m_music_enabled)
    return;

  if (filename.empty()) {
    m_device->stop_music(0);
    return;
  }

  try {
    m_device->play_music(filename, fadetime, static_cast<float>(m_music_volume) / 100.0f);
  } catch(std::exception& e) {
    log_warning << "Couldn't play music file '" << filename << "': " << e.what() << std::endl;
    // When this happens, previous music continued playing, stop it, just in case.
    stop_music(0);
  }
}

void
SoundManager::play_music(const std::string& filename, bool fade)
{
  play_music(filename, fade ? 0.5f : 0);
}

void
SoundManager::pause_music(float fadetime)
{
  m_device->pause_music(fadetime);
}

void
SoundManager::pause_sounds()
{
  for (auto& source : m_sources) {
    if (source->playing()) {
      source->pause();
    }
  }
}

void
SoundManager::resume_sounds()
{
  for (auto& source : m_sources) {
    if (source->paused()) {
      source->resume();
    }
  }
}

void
SoundManager::stop_sounds()
{
  for (auto& source : m_sources) {
    source->stop();
  }
}

void
SoundManager::set_sound_volume(int volume)
{
  m_sound_volume = volume;
  for (auto& source : m_sources) {
    source->set_volume(static_cast<float>(volume) / 100.0f);
  }
}

void
SoundManager::resume_music(float fadetime)
{
  m_device->resume_music(fadetime);
}

void
SoundManager::set_listener_position(const Vector& pos)
{
  static Uint32 lastticks = SDL_GetTicks();

  Uint32 current_ticks = SDL_GetTicks();
  if (current_ticks - lastticks < 300)
    return;
  lastticks = current_ticks;

  m_device->set_listener_position(pos);
}

void
SoundManager::set_player_position(const Vector& position)
{
  m_player_position = position;
  for (auto& source : m_sources) {
    source->update_placement();
  }
}

void
SoundManager::set_listener_orientation(const Vector& at, const Vector& up)
{
  m_device->set_listener_orientation(at, up);
}

void
SoundManager::get_placement(const Vector& position, bool close, float close_range, bool full, float& left, float& right) const
{
  const float dx = position.x - m_player_position.x;
  const float dy = position.y - m_player_position.y;

  // Fades in a straight line from beside Tux to nothing at the sound's range
  const float range = close ? close_range : PLACED_RANGE;
  const float distance = std::sqrt(dx * dx + dy * dy);
  const float fade = std::clamp(1.0f - (distance - BESIDE_TUX) / (range - BESIDE_TUX), 0.0f, 1.0f);

  float near_ear, far_ear;
  if (!full) {
    // Vanilla's own fall off and pan for a mono sound, so it carries and leans the way it did before fading out
    const float reference = close ? VANILLA_CLOSE_REFERENCE : VANILLA_PLACED_REFERENCE;
    const float heard = reference / std::sqrt(dx * dx + dy * dy + VANILLA_SETBACK * VANILLA_SETBACK);
    vanilla_pan(dx, near_ear, far_ear);
    near_ear *= heard * fade;
    far_ear *= heard * fade;
  } else {
    // A stereo sound vanilla never placed keeps its full volume beside Tux, and the far ear fades out gently
    const float lean = std::clamp((std::abs(dx) - BESIDE_TUX) / (range - BESIDE_TUX), 0.0f, 1.0f);
    near_ear = FULL_LEVEL * fade;
    far_ear = near_ear * (1.0f - lean);
  }

  far_ear = std::max(far_ear, near_ear * FAR_EAR_FLOOR);
  left = (dx > 0.0f) ? far_ear : near_ear;
  right = (dx > 0.0f) ? near_ear : far_ear;
}

void
SoundManager::get_lean(const Vector& position, float& left, float& right) const
{
  const float dx = position.x - m_player_position.x;
  float near_ear, far_ear;
  vanilla_pan(dx, near_ear, far_ear);
  far_ear = std::max(far_ear, near_ear * FAR_EAR_FLOOR);
  left = (dx > 0.0f) ? far_ear : near_ear;
  right = (dx > 0.0f) ? near_ear : far_ear;
}

void
SoundManager::update()
{
  static Uint32 lasttime = SDL_GetTicks();
  Uint32 now = SDL_GetTicks();

  if (now - lasttime < 300)
    return;
  lasttime = now;

  // update and check for finished sound sources
  for (auto it = m_sources.begin(); it != m_sources.end(); ) {
    auto& source = *it;

    source->update();

    // A paused sound isn't finished, unpausing the game resumes it
    if (!source->playing() && !source->paused()) {
      it = m_sources.erase(it);
    } else {
      ++it;
    }
  }
  m_device->update();

  //run update() for stream_sound_source
  auto s = m_update_list.begin();
  while (s != m_update_list.end()) {
    (*s)->update();
    ++s;
  }
}

/* EOF */
