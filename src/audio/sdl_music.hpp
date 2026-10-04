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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_MUSIC_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_MUSIC_HPP

#include <atomic>
#include <memory>

#include <SDL_stdinc.h>

#include "audio/sdl_stream.hpp"

class SoundFile;

/** The music on SDL_mixer, playing, pausing and fading the way StreamSoundSource does on OpenAL */
class SDLMusic final
{
public:
  enum FadeState { NoFading, FadingOn, FadingOff, FadingPause, FadingResume };

public:
  SDLMusic(std::unique_ptr<SoundFile> file, int rate);

  void play();
  void stop();
  void pause();
  void resume();
  bool playing() const;
  bool paused() const;

  void set_volume(float volume);
  void set_fading(FadeState state, float fadetime);
  FadeState get_fade_state() const { return m_fade_state; }

  /** Reads ahead and moves any fade on, between frames */
  void update();

  /** Hands SDL_mixer the next stretch of music, on its own thread */
  static void SDLCALL feed(void* music, Uint8* stream, int len);

private:
  enum class State { INITIAL, PLAYING, PAUSED, STOPPED };

  void set_gain(float gain);
  void send_level();

private:
  SDLStream m_stream;
  State m_state;
  FadeState m_fade_state;
  float m_fade_start_time;
  float m_fade_time;
  float m_gain;
  float m_volume;

  /** What SDL_mixer's thread plays, and the level it's moving from */
  std::atomic<bool> m_running;
  std::atomic<float> m_level;
  float m_heard;

private:
  SDLMusic(const SDLMusic&) = delete;
  SDLMusic& operator=(const SDLMusic&) = delete;
};

#endif

/* EOF */
