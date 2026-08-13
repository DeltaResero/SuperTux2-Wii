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

#include <SDL.h>
#include <cmath>
#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <memory>

#include "audio/dummy_sound_source.hpp"
#include "audio/openal_device.hpp"
#include "audio/sound_file.hpp"
#include "audio/stream_sound_source.hpp"
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
   are measured along the ground, and the setback is added back before these
   reach OpenAL, which measures from the listener. Giving a close sound a
   silence distance directly instead was the mistake that made the fuse
   inaudible: the setback swallowed all but a few tiles of its range. */
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

} // namespace

SoundManager::SoundManager() :
  m_device(new OpenALDevice),
  sound_enabled(false),
  sources(),
  update_list(),
  music_source(),
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
  music_source.reset();
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
SoundManager::register_for_update(StreamSoundSource* sss)
{
  if (sss)
  {
    update_list.push_back(sss);
  }
}

void
SoundManager::remove_from_update(StreamSoundSource* sss)
{
  if (sss)
  {
    StreamSoundSources::iterator i = update_list.begin();
    while( i != update_list.end() ){
      if( *i == sss ){
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
    if(music_source) {
      music_source.reset();
    }
  }
}

void
SoundManager::stop_music(float fadetime)
{
  if(fadetime > 0) {
    if(music_source
       && music_source->get_fade_state() != StreamSoundSource::FadingOff)
      music_source->set_fading(StreamSoundSource::FadingOff, fadetime);
  } else {
    music_source.reset();
  }
  current_music = "";
}

void
SoundManager::play_music(const std::string& filename, bool fade)
{
  if(filename == current_music && music_source != NULL)
  {
    if(music_source->paused())
    {
      music_source->resume();
    }
    else if(!music_source->playing())
    {
      music_source->play();
    }
    return;
  }
  current_music = filename;
  if(!music_enabled)
    return;

  if(filename.empty()) {
    music_source.reset();
    return;
  }

  try {
    std::unique_ptr<StreamSoundSource> newmusic (new StreamSoundSource());
    newmusic->set_sound_file(load_sound_file(filename));
    newmusic->set_looping(true);
    newmusic->set_relative(true);
    if(fade)
      newmusic->set_fading(StreamSoundSource::FadingOn, .5f);
    newmusic->play();

    music_source = std::move(newmusic);
  } catch(std::exception& e) {
    log_warning << "Couldn't play music file '" << filename << "': " << e.what() << std::endl;
    // When this happens, previous music continued playing, stop it, just in case.
    stop_music(0);
  }
}

void
SoundManager::pause_music(float fadetime)
{
  if(music_source == NULL)
    return;

  if(fadetime > 0) {
    if(music_source
       && music_source->get_fade_state() != StreamSoundSource::FadingPause)
      music_source->set_fading(StreamSoundSource::FadingPause, fadetime);
  } else {
    music_source->pause();
  }
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
  if(music_source == NULL)
    return;

  if(fadetime > 0) {
    if(music_source
       && music_source->get_fade_state() != StreamSoundSource::FadingResume)
      music_source->set_fading(StreamSoundSource::FadingResume, fadetime);
  } else {
    music_source->resume();
  }
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
  // check streaming sounds
  if(music_source) {
    music_source->update();
  }

  m_device->update();

  //run update() for stream_sound_source
  StreamSoundSources::iterator s = update_list.begin();
  while( s != update_list.end() ){
    (*s)->update();
    ++s;
  }
}





/* EOF */
