// src/audio/sound_manager.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include <config.h>

#include <SDL.h>
#include <cmath>
#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <exception>
#include <utility>

#include "audio/dummy_sound_source.hpp"
#ifdef ENABLE_OPENAL
#include "audio/openal_device.hpp"
#endif
#ifdef ENABLE_SDL_MIXER
#include "audio/sdl_mixer_device.hpp"
#endif
#include "audio/sound_file.hpp"
#include "audio/sound_source.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"

namespace {

/* Sound fades to nothing exactly where the engine stops running a badguy,
   BadGuy::X_OFFSCREEN_DISTANCE. Beyond that a bouncing Mr Iceblock is simply
   switched off, so anything still audible at that point stops mid sound
   rather than fading. That distance is a fixed count of world units and not
   a share of the view, so these follow it rather than the window size.

   The listener is held back from the plane the game is drawn on so that a
   sound beside Tux does not pan hard into one ear. */
const float SILENCE_DISTANCE = 1280.0f;
const float LISTENER_SETBACK = 300.0f;

/* Levels, as fractions of an unplaced sound. A placed sound is part of the
   scene rather than something happening to the player, so it sits under one.

   A close sound, a ticking fuse or a flame, gets its own pair: how loud it is
   when Tux is standing on it, and how far along the ground it carries. Both
   are measured along the ground, and the setback is added back by whichever
   device is underneath, since a device measures from the listener. Giving a
   close sound a silence distance directly instead was the mistake that made
   the fuse inaudible: the setback swallowed all but a few tiles of its
   range. */
const float PLACED_LEVEL = 0.7f;
const float CLOSE_LEVEL  = 0.5f;
const float CLOSE_CARRY  = 480.0f;

/** Distance at which a placed sound reaches silence, measured from the
    listener rather than along the ground, since the setback above means the
    two are not the same. */
float silence_range()
{
  return std::sqrt(SILENCE_DISTANCE * SILENCE_DISTANCE
                 + LISTENER_SETBACK * LISTENER_SETBACK);
}

/** Build whichever backend was asked for, out of the ones this build has.
    AUTO favours OpenAL when both are here, so that leaving the flag alone
    keeps the desktop sounding as it did. */
std::unique_ptr<AudioDevice> open_device(AudioBackend wanted)
{
#ifdef ENABLE_SDL_MIXER
  if (wanted == AudioBackend::SdlMixer)
    return std::unique_ptr<AudioDevice>(new SDLMixerDevice);
#endif
#ifdef ENABLE_OPENAL
  if (wanted == AudioBackend::OpenAL || wanted == AudioBackend::Automatic)
    return std::unique_ptr<AudioDevice>(new OpenALDevice);
#endif
#ifdef ENABLE_SDL_MIXER
  /* Only reachable when this build has no OpenAL, which is the console. */
  return std::unique_ptr<AudioDevice>(new SDLMixerDevice);
#else
  return std::unique_ptr<AudioDevice>(new OpenALDevice);
#endif
}

} // namespace

SoundManager::SoundManager() :
  m_device(open_device(g_config ? g_config->audio_backend : AudioBackend::Automatic)),
  sound_enabled(false),
  sources(),
  update_list(),
  music_enabled(false),
  current_music()
{
  sound_enabled = m_device->is_open();
  music_enabled = m_device->is_open();

  if (m_device->is_open())
    set_listener_orientation(Vector(0.0f, 0.0f), Vector(0.0f, -1.0f));
}

SoundManager::~SoundManager()
{
  /* Sources hold buffers the device owns, so they have to go first. */
  sources.clear();
  m_device.reset();
}

bool
SoundManager::is_audio_enabled() const
{
  return m_device && m_device->is_open();
}

std::unique_ptr<SoundSource>
SoundManager::create_sound_source(const std::string& filename)
{
  if(!sound_enabled)
    return create_dummy_sound_source();

  try {
    return m_device->create_source(filename);
  } catch(std::exception &e) {
    log_warning << "Couldn't create audio source: " << e.what() << std::endl;
    return create_dummy_sound_source();
  }
}

void
SoundManager::preload(const std::string& filename)
{
  if(!sound_enabled)
    return;

  m_device->preload(filename);
}

void
SoundManager::play(const std::string& filename, const Vector& pos)
{
  if(!sound_enabled)
    return;

  try {
    std::unique_ptr<SoundSource> source(m_device->create_source(filename));

    if(pos.x < 0 || pos.y < 0) {
      source->set_relative(true);
    } else {
      source->set_placed_range();
      source->set_position(pos);
    }
    source->play();
    sources.push_back(std::move(source));
  } catch(std::exception& e) {
    log_warning << "Couldn't play sound " << filename << ": " << e.what() << std::endl;
  }
}

void
SoundManager::manage_source(std::unique_ptr<SoundSource> source)
{
  assert(source);
  sources.push_back(std::move(source));
}

void
SoundManager::register_for_update(SoundSource* source)
{
  if (source)
  {
    update_list.push_back(source);
  }
}

void
SoundManager::remove_from_update(SoundSource* source)
{
  if (source)
  {
    UpdateList::iterator i = update_list.begin();
    while( i != update_list.end() ){
      if( *i == source ){
        i = update_list.erase(i);
      } else {
        ++i;
      }
    }
  }
}

void
SoundManager::enable_sound(bool enable)
{
  if(!is_audio_enabled())
    return;

  sound_enabled = enable;
}

void
SoundManager::enable_music(bool enable)
{
  if(!is_audio_enabled())
    return;

  music_enabled = enable;
  if(music_enabled) {
    play_music(current_music);
  } else {
    m_device->stop_music(0);
  }
}

void
SoundManager::stop_music(float fadetime)
{
  m_device->stop_music(fadetime);
  current_music = "";
}

void
SoundManager::play_music(const std::string& filename, bool fade)
{
  if(filename == current_music && m_device->has_music())
  {
    /* Already the right track, so see it running rather than start it over. */
    m_device->resume_music(0);
    return;
  }

  /* Remembered even when music is off, so that turning it back on picks up
     where the game meant to be rather than in silence. */
  current_music = filename;
  if(!music_enabled)
    return;

  if(filename.empty()) {
    m_device->stop_music(0);
    return;
  }

  try {
    m_device->play_music(filename, fade ? .5f : 0.0f);
  } catch(std::exception& e) {
    log_warning << "Couldn't play music file '" << filename << "': " << e.what() << std::endl;
    // When this happens, previous music continued playing, stop it, just in case.
    stop_music(0);
  }
}

void
SoundManager::pause_music(float fadetime)
{
  m_device->pause_music(fadetime);
}

void
SoundManager::pause_sounds()
{
  for(auto& source : sources) {
    if(source->playing()) {
      source->pause();
    }
  }
}

void
SoundManager::resume_sounds()
{
  for(auto& source : sources) {
    if(source->paused()) {
      source->resume();
    }
  }
}

void
SoundManager::stop_sounds()
{
  for(auto& source : sources) {
    source->stop();
  }
}

void
SoundManager::resume_music(float fadetime)
{
  m_device->resume_music(fadetime);
}

float
SoundManager::placed_level()
{
  return PLACED_LEVEL;
}

float
SoundManager::placed_silence()
{
  return silence_range();
}

float
SoundManager::close_level()
{
  return CLOSE_LEVEL;
}

float
SoundManager::listener_setback()
{
  return LISTENER_SETBACK;
}

float
SoundManager::close_silence()
{
  return std::sqrt(CLOSE_CARRY * CLOSE_CARRY + LISTENER_SETBACK * LISTENER_SETBACK);
}

void
SoundManager::set_listener_position(const Vector& pos)
{
  static Uint32 lastticks = SDL_GetTicks();

  Uint32 current_ticks = SDL_GetTicks();
  if(current_ticks - lastticks < 300)
    return;
  lastticks = current_ticks;

  m_device->set_listener_position(pos);
}

void
SoundManager::set_listener_velocity(const Vector& vel)
{
  m_device->set_listener_velocity(vel);
}

void
SoundManager::set_listener_orientation(const Vector& at, const Vector& up)
{
  m_device->set_listener_orientation(at, up);
}

void
SoundManager::update()
{
  /* The device runs every frame. The SDL_mixer device watches the music's
     position to seek at loop points, and the position only shows the end of
     a track for about twenty milliseconds, so a slower cadence misses it.
     Only the source reaping below stays on the old throttle. */
  m_device->update();

  static Uint32 lasttime = SDL_GetTicks();
  Uint32 now = SDL_GetTicks();

  if(now - lasttime < 300)
    return;
  lasttime = now;

  // update and check for finished sound sources
  for(SoundSources::iterator i = sources.begin(); i != sources.end(); ) {
    auto& source = *i;

    source->update();

    if(!source->playing()) {
      i = sources.erase(i);
    } else {
      ++i;
    }
  }

  /* Sources the caller holds rather than this manager, so they are not in
     the list above and would otherwise never refill. */
  for(auto* source : update_list) {
    source->update();
  }
}





/* EOF */
