// src/audio/sound_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP

#include <config.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#ifdef HAVE_OPENAL
#include <al.h>
#include <alc.h>
#endif

#ifdef USE_SDL_MIXER
#include <SDL_mixer.h>
#endif

#include "math/vector.hpp"
#include "util/currenton.hpp"

class SoundFile;
class SoundSource;
class StreamSoundSource;
class OpenALSoundSource;

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

#ifdef HAVE_OPENAL
  /*
   * Tell soundmanager to call update() for stream_sound_source.
   */
  void register_for_update( StreamSoundSource* sss );
  /*
   * Unsubscribe from updates for stream_sound_source.
   */
  void remove_from_update( StreamSoundSource* sss );
#endif

#ifdef USE_SDL_MIXER
  void clear_music_cache();
  void clear_sound_cache();

  // Needed by MusicRef
  struct MusicResource {
      SoundManager* manager;
      Mix_Music* music;
      int refcount;
  };
  void free_music(MusicResource* music);
#endif

private:
  friend class OpenALSoundSource;
  friend class StreamSoundSource;
  friend class MusicRef;

#ifdef HAVE_OPENAL
  /** creates a new sound source, might throw exceptions, never returns NULL */
  std::unique_ptr<OpenALSoundSource> intern_create_sound_source(const std::string& filename);
  static ALuint load_file_into_buffer(SoundFile& file);
  static ALenum get_sample_format(const SoundFile& file);

  static void print_openal_version();
  void check_alc_error(const char* message) const;
  static void check_al_error(const char* message);

  ALCdevice* device;
  ALCcontext* context;

  typedef std::map<std::string, ALuint> SoundBuffers;
  SoundBuffers buffers;

  std::unique_ptr<StreamSoundSource> music_source;

  typedef std::vector<StreamSoundSource*> StreamSoundSources;
  StreamSoundSources update_list;
#endif

#ifdef USE_SDL_MIXER
  typedef std::map<std::string, Mix_Chunk*> SoundChunks;
  SoundChunks sound_chunks;

  typedef std::map<std::string, MusicResource> Musics;
  Musics musics;

  MusicResource* current_music_resource;
#endif

  bool sound_enabled;
  bool music_enabled;
  std::string current_music;

  typedef std::vector<std::unique_ptr<SoundSource> > SoundSources;
  SoundSources sources;

private:
  SoundManager(const SoundManager&);
  SoundManager& operator=(const SoundManager&);
};

#endif // HEADER_SUPERTUX_AUDIO_SOUND_MANAGER_HPP

// EOF
