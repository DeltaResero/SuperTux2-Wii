// src/audio/sdl_mixer_device.cpp
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

#include "audio/sdl_mixer_device.hpp"

#include <stdexcept>

#include "audio/sdl_sound_source.hpp"
#include "audio/sound_error.hpp"
#include "audio/sound_file.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"

namespace {

/* Knobs worth revisiting on the console rather than the desktop. A larger
   fragment costs latency and buys mixing time; fewer channels cost polyphony
   and buy the same. These are the desktop-comfortable end of both. */
const int MIXER_RATE      = 44100;
const int MIXER_CHANNELS  = 2;
const int MIXER_FRAGMENT  = 1024;
const int MIXING_CHANNELS = 32;

} // namespace

SDLMixerDevice::SDLMixerDevice() :
  m_open(false),
  m_chunks(),
  m_music(nullptr),
  m_loop_begin(0.0f),
  m_loop_jump_at(0.0f),
  m_listener(0.0f, 0.0f)
{
  if(Mix_OpenAudio(MIXER_RATE, MIX_DEFAULT_FORMAT, MIXER_CHANNELS, MIXER_FRAGMENT) < 0)
  {
    log_warning << "Couldn't open audio device: " << Mix_GetError() << std::endl;
    return;
  }

  Mix_AllocateChannels(MIXING_CHANNELS);
  m_open = true;
}

SDLMixerDevice::~SDLMixerDevice()
{
  if(!m_open)
    return;

  /* Sounds have to stop before the chunks they are playing go. */
  Mix_HaltChannel(-1);
  release_music();

  for(const auto& chunk : m_chunks) {
    Mix_FreeChunk(chunk.second);
  }
  m_chunks.clear();

  Mix_CloseAudio();
}

Mix_Chunk*
SDLMixerDevice::chunk_for(const std::string& filename)
{
  std::map<std::string, Mix_Chunk*>::iterator held = m_chunks.find(filename);
  if(held != m_chunks.end())
    return held->second;

  const std::string path = FileSystem::find(filename);
  if(path.empty())
    throw SoundError("Couldn't open '" + filename + "': not found");

  Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
  if(chunk == nullptr)
    throw SoundError("Couldn't read '" + path + "': " + Mix_GetError());

  m_chunks.insert(std::make_pair(filename, chunk));
  return chunk;
}

std::unique_ptr<SoundSource>
SDLMixerDevice::create_source(const std::string& filename)
{
  return std::unique_ptr<SoundSource>(new SDLSoundSource(*this, chunk_for(filename)));
}

void
SDLMixerDevice::preload(const std::string& filename)
{
  try {
    chunk_for(filename);
  } catch(std::exception& e) {
    log_warning << "Error while preloading sound file: " << e.what() << std::endl;
  }
}

void
SDLMixerDevice::release_music()
{
  if(m_music != nullptr) {
    Mix_HaltMusic();
    Mix_FreeMusic(m_music);
    m_music = nullptr;
  }
}

void
SDLMixerDevice::play_music(const std::string& filename, float fade_in)
{
  /* A .music file names the audio and says where it repeats. Anything else
     is taken as the audio itself, looping whole. */
  std::string audio = filename;
  float loop_begin = 0.0f;
  float loop_at    = -1.0f;

  if(filename.length() > 6
     && filename.compare(filename.length() - 6, 6, ".music") == 0)
  {
    const MusicReference reference = load_music_reference(filename);
    audio      = reference.file;
    loop_begin = reference.loop_begin;
    loop_at    = reference.loop_at;
  }

  const std::string path = FileSystem::find(audio);
  if(path.empty())
    throw SoundError("Couldn't open '" + audio + "': not found");

  Mix_Music* track = Mix_LoadMUS(path.c_str());
  if(track == nullptr)
    throw SoundError("Couldn't read '" + path + "': " + Mix_GetError());

  release_music();
  m_music = track;
  m_loop_begin = loop_begin;

  /* SDL_mixer only knows how to repeat a whole file. Where the game wants an
     intro played once, or wants the repeat to start before the file runs
     out, the position is watched instead and moved back at the right second.
     A track that repeats whole needs none of that and is left to SDL. */
  if(loop_begin > 0.0f)
  {
    m_loop_jump_at = (loop_at >= 0.0f) ? loop_at
                                       : static_cast<float>(Mix_MusicDuration(m_music));
  }
  else if(loop_at >= 0.0f)
  {
    m_loop_jump_at = loop_at;
  }
  else
  {
    m_loop_jump_at = 0.0f;
  }

  /* Played endlessly either way, so that a missed second near the end costs
     one extra turn through the intro rather than silence. */
  const int repeats = -1;
  const int started = (fade_in > 0.0f)
    ? Mix_FadeInMusic(m_music, repeats, static_cast<int>(fade_in * 1000.0f))
    : Mix_PlayMusic(m_music, repeats);

  if(started < 0)
  {
    const std::string trouble = Mix_GetError();
    release_music();
    throw SoundError("Couldn't play '" + path + "': " + trouble);
  }
}

void
SDLMixerDevice::stop_music(float fade_out)
{
  if(m_music == nullptr)
    return;

  if(fade_out > 0.0f)
  {
    if(Mix_FadingMusic() != MIX_FADING_OUT)
      Mix_FadeOutMusic(static_cast<int>(fade_out * 1000.0f));
  }
  else
  {
    release_music();
  }
}

void
SDLMixerDevice::pause_music(float fade_out)
{
  if(m_music == nullptr)
    return;

  /* SDL_mixer cannot fade to a pause, only to a stop, so a fade here ends
     with the track gone rather than held. Nothing asks to resume after a
     faded pause without naming the track again, so it comes back either
     way. */
  if(fade_out > 0.0f)
  {
    if(Mix_FadingMusic() != MIX_FADING_OUT)
      Mix_FadeOutMusic(static_cast<int>(fade_out * 1000.0f));
  }
  else
  {
    Mix_PauseMusic();
  }
}

void
SDLMixerDevice::resume_music(float fade_in)
{
  if(m_music == nullptr)
    return;

  if(Mix_PausedMusic())
  {
    Mix_ResumeMusic();
  }
  else if(!Mix_PlayingMusic())
  {
    if(fade_in > 0.0f)
      Mix_FadeInMusic(m_music, -1, static_cast<int>(fade_in * 1000.0f));
    else
      Mix_PlayMusic(m_music, -1);
  }
}

void
SDLMixerDevice::follow_loop_point()
{
  if(m_music == nullptr || m_loop_jump_at <= 0.0f || !Mix_PlayingMusic())
    return;

  const double at = Mix_GetMusicPosition(m_music);
  if(at < 0.0)
    return;   // this format cannot say where it is

  if(at >= static_cast<double>(m_loop_jump_at))
    Mix_SetMusicPosition(static_cast<double>(m_loop_begin));
}

void
SDLMixerDevice::set_listener_position(const Vector& position)
{
  m_listener = position;
}

void
SDLMixerDevice::set_listener_velocity(const Vector&)
{
  /* For a doppler shift, which wants a pitch control SDL_mixer has not got.
     Nothing in the game sets a velocity on a sound. */
}

void
SDLMixerDevice::set_listener_orientation(const Vector&, const Vector&)
{
  /* The listener always faces into the screen with the world flat in front
     of it, so there is nothing here to turn. */
}

void
SDLMixerDevice::update()
{
  follow_loop_point();
}

/* EOF */
