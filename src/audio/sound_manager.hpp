// src/audio/sound_manager.hpp
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

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>

#include "math/vector.hpp"
#include "util/currenton.hpp"

class AudioDevice;
class SoundFile;
class SoundSource;

class SoundManager : public Currenton<SoundManager>
{
public:
  SoundManager();
  virtual ~SoundManager();

  void enable_sound(bool sound_enabled);
  /**
   * Creates a new sound source object which plays the specified soundfile.
   * You are responsible for deleting the sound source later (this will stop the
   * sound).
   * This function never throws exceptions, but might return a DummySoundSource
   */
  std::unique_ptr<SoundSource> create_sound_source(const std::string& filename);
  /**
   * Convenience function to simply play a sound at a given position.
   */
  void play(const std::string& name, const Vector& pos = Vector(-1, -1));
  /**
   * Adds the source to the list of managed sources (= the source gets deleted
   * when it finished playing)
   */
  void manage_source(std::unique_ptr<SoundSource> source);
  /// preloads a sound, so that you don't get a lag later when playing it
  void preload(const std::string& name);

  /** The figures the fall off is built from, kept here so they all sit in
      one file. A sound holds its level out as far as the listener stands
      back, then fades to nothing by the matching silence distance. */
  static float listener_setback();
  static float placed_level();
  static float placed_silence();
  static float close_level();
  static float close_silence();

  void set_listener_position(const Vector& position);
  void set_listener_velocity(const Vector& velocity);
  void set_listener_orientation(const Vector& at, const Vector& up);

  void enable_music(bool music_enabled);
  void play_music(const std::string& filename, bool fade = false);
  void pause_music(float fadetime = 0);
  void resume_music(float fadetime = 0);
  void stop_music(float fadetime = 0);

  void pause_sounds();
  void resume_sounds();
  void stop_sounds();

  bool is_music_enabled() const { return music_enabled; }
  bool is_sound_enabled() const { return sound_enabled; }

  bool is_audio_enabled() const;
  std::string get_current_music() const {
    return current_music;
  }
  void update();

  /** Ask to be updated every frame. For a source the caller holds itself,
      which is therefore not in the list of sources this manages. */
  void register_for_update( SoundSource* source );
  void remove_from_update( SoundSource* source );

private:
  /** The hardware and the library that drives it. Never null once built, but
      ask is_open() before expecting anything of it. */
  std::unique_ptr<AudioDevice> m_device;

  bool sound_enabled;

  typedef std::vector<std::unique_ptr<SoundSource> > SoundSources;
  SoundSources sources;

  typedef std::vector<SoundSource*> UpdateList;
  UpdateList update_list;

  bool music_enabled;
  std::string current_music;

private:
  SoundManager(const SoundManager&);
  SoundManager& operator=(const SoundManager&);
};

#endif

/* EOF */
