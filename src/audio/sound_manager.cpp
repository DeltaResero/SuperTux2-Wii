// src/audio/sound_manager.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2025 DeltaResero
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "audio/sound_manager.hpp"

#include <SDL.h>
#include <algorithm>
#include <assert.h>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "audio/dummy_sound_source.hpp"
#include "audio/sound_file.hpp"
#include "util/log.hpp"

#ifdef HAVE_OPENAL
#include "audio/openal_sound_source.hpp"
#include "audio/stream_sound_source.hpp"
#endif

#ifdef USE_SDL_MIXER
#include "audio/sdl_sound_source.hpp"
#include "io/sdl_io.hpp"
#endif

SoundManager::SoundManager()
    :
#ifdef HAVE_OPENAL
      device(nullptr), context(nullptr), buffers(), music_source(),
      update_list(),
#endif
#ifdef USE_SDL_MIXER
      sound_chunks(), musics(), current_music_resource(nullptr),
#endif
      sound_enabled(false), music_enabled(false), current_music(), sources() {
#ifdef HAVE_OPENAL
  try {
    device = alcOpenDevice(nullptr);
    if (device == nullptr) {
      throw std::runtime_error("Couldn't open audio device.");
    }

    int attributes[] = {0};
    context = alcCreateContext(device, attributes);
    check_alc_error("Couldn't create audio context: ");
    alcMakeContextCurrent(context);
    check_alc_error("Couldn't select audio context: ");

    check_al_error("Audio error after init: ");
    sound_enabled = true;
    music_enabled = true;

    set_listener_orientation(Vector(0.0f, 0.0f), Vector(0.0f, -1.0f));
  } catch (std::exception &e) {
    if (context != nullptr) {
      alcDestroyContext(context);
      context = nullptr;
    }
    if (device != nullptr) {
      alcCloseDevice(device);
      device = nullptr;
    }
    log_warning << "Couldn't initialize audio device: " << e.what()
                << std::endl;
    print_openal_version();
  }
#endif

#ifdef USE_SDL_MIXER
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    log_warning << "Couldn't initialize SDL Audio subsystem: " << SDL_GetError()
                << std::endl;
  }

  // Initialize OGG support explicitly
  int flags = MIX_INIT_OGG;
  int inited = Mix_Init(flags);
  if ((inited & flags) != flags) {
    log_warning << "Mix_Init: Failed to init required ogg support: "
                << Mix_GetError() << std::endl;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    log_warning << "Couldn't initialize SDL_mixer: " << Mix_GetError()
                << std::endl;
    sound_enabled = false;
  } else {
    Mix_AllocateChannels(32);
    sound_enabled = true;
    music_enabled = true;
  }
#endif
}

SoundManager::~SoundManager() {
  sources.clear();

#ifdef HAVE_OPENAL
  music_source.reset();

  for (const auto &buffer : buffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  if (context != nullptr) {
    alcDestroyContext(context);
    context = nullptr;
  }
  if (device != nullptr) {
    alcCloseDevice(device);
    device = nullptr;
  }
#endif

#ifdef USE_SDL_MIXER
  Mix_HaltMusic();
  Mix_HaltChannel(-1);

  for (auto &chunk_pair : sound_chunks) {
    Mix_FreeChunk(chunk_pair.second);
  }
  sound_chunks.clear();

  if (current_music_resource) {
    current_music_resource->refcount--;
    if (current_music_resource->refcount <= 0) {
      free_music(current_music_resource);
    }
    current_music_resource = nullptr;
  }

  for (auto &m : musics) {
    if (m.second.music) {
      Mix_FreeMusic(m.second.music);
      m.second.music = nullptr;
    }
  }
  musics.clear();

  Mix_CloseAudio();
  Mix_Quit();
#endif
}

#ifdef HAVE_OPENAL
ALuint SoundManager::load_file_into_buffer(SoundFile &file) {
  ALenum format = get_sample_format(file);
  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_error("Couldn't create audio buffer: ");
  std::unique_ptr<char[]> samples(new char[file.size]);
  file.read(samples.get(), file.size);
  alBufferData(buffer, format, samples.get(), static_cast<ALsizei>(file.size),
               static_cast<ALsizei>(file.rate));
  check_al_error("Couldn't fill audio buffer: ");

  return buffer;
}

std::unique_ptr<OpenALSoundSource>
SoundManager::intern_create_sound_source(const std::string &filename) {
  assert(sound_enabled);

  std::unique_ptr<OpenALSoundSource> source(new OpenALSoundSource);

  ALuint buffer;

  // reuse an existing static sound buffer
  SoundBuffers::iterator i = buffers.find(filename);
  if (i != buffers.end()) {
    buffer = i->second;
  } else {
    // Load sound file
    std::unique_ptr<SoundFile> file(load_sound_file(filename));

    if (file->size < 100000) {
      buffer = load_file_into_buffer(*file);
      buffers.insert(std::make_pair(filename, buffer));
    } else {
      std::unique_ptr<StreamSoundSource> source_(new StreamSoundSource);
      source_->set_sound_file(std::move(file));
      return std::move(source_);
    }
  }

  alSourcei(source->source, AL_BUFFER, buffer);
  return source;
}
#endif

std::unique_ptr<SoundSource>
SoundManager::create_sound_source(const std::string &filename) {
  if (!sound_enabled)
    return create_dummy_sound_source();

#ifdef HAVE_OPENAL
  try {
    return intern_create_sound_source(filename);
  } catch (std::exception &e) {
    log_warning << "Couldn't create audio source: " << e.what() << std::endl;
    return create_dummy_sound_source();
  }
#endif

#ifdef USE_SDL_MIXER
  Mix_Chunk *chunk = nullptr;
  auto it = sound_chunks.find(filename);

  if (it != sound_chunks.end()) {
    chunk = it->second;
  } else {
    try {
      chunk = Mix_LoadWAV_RW(get_physfs_SDLRWops(filename), 1);
      if (!chunk) {
        log_warning << "Couldn't load sound '" << filename
                    << "': " << Mix_GetError() << std::endl;
        return create_dummy_sound_source();
      }
      sound_chunks[filename] = chunk;
    } catch (std::exception &e) {
      log_warning << "Couldn't load sound '" << filename << "': " << e.what()
                  << std::endl;
      return create_dummy_sound_source();
    }
  }

  return std::unique_ptr<SoundSource>(new SDLSoundSource(chunk));
#endif

  return create_dummy_sound_source();
}

void SoundManager::preload(const std::string &filename) {
  if (!sound_enabled)
    return;

#ifdef USE_SDL_MIXER
  // Skip preloading on SDL_mixer builds to save memory on low-end systems.
  // Sounds will load on-demand when first played, then cache until scene ends.
  (void)filename;
  return;
#endif

#ifdef HAVE_OPENAL
  SoundBuffers::iterator i = buffers.find(filename);
  // already loaded?
  if (i != buffers.end())
    return;
  try {
    std::unique_ptr<SoundFile> file(load_sound_file(filename));
    // only keep small files
    if (file->size >= 100000)
      return;

    ALuint buffer = load_file_into_buffer(*file);
    buffers.insert(std::make_pair(filename, buffer));
  } catch (std::exception &e) {
    log_warning << "Error while preloading sound file: " << e.what()
                << std::endl;
  }
#endif

#ifdef USE_SDL_MIXER
  if (sound_chunks.find(filename) == sound_chunks.end()) {
    try {
      Mix_Chunk *chunk = Mix_LoadWAV_RW(get_physfs_SDLRWops(filename), 1);
      if (chunk) {
        sound_chunks[filename] = chunk;
      }
    } catch (std::exception &e) {
      log_warning << "Error while preloading sound file: " << e.what()
                  << std::endl;
    }
  }
#endif
}

void SoundManager::play(const std::string &filename, const Vector &pos) {
  if (!sound_enabled)
    return;

  try {
    std::unique_ptr<SoundSource> source = create_sound_source(filename);
    if (!source)
      return;

    if (pos.x < 0 || pos.y < 0) {
#ifdef HAVE_OPENAL
      if (auto *openal_source =
              dynamic_cast<OpenALSoundSource *>(source.get())) {
        openal_source->set_relative(true);
      }
#endif
      // SDL_mixer source doesn't support relative setting easily, ignore
    } else {
      source->set_position(pos);
    }
    source->play();
    manage_source(std::move(source));
  } catch (std::exception &e) {
    log_warning << "Couldn't play sound " << filename << ": " << e.what()
                << std::endl;
  }
}

void SoundManager::manage_source(std::unique_ptr<SoundSource> source) {
  assert(source);
  sources.push_back(std::move(source));
}

#ifdef HAVE_OPENAL
void SoundManager::register_for_update(StreamSoundSource *sss) {
  if (sss) {
    update_list.push_back(sss);
  }
}

void SoundManager::remove_from_update(StreamSoundSource *sss) {
  if (sss) {
    StreamSoundSources::iterator i = update_list.begin();
    while (i != update_list.end()) {
      if (*i == sss) {
        i = update_list.erase(i);
      } else {
        ++i;
      }
    }
  }
}
#endif

void SoundManager::enable_sound(bool enable) {
#ifdef HAVE_OPENAL
  if (device == nullptr)
    return;
#endif

  sound_enabled = enable;
}

void SoundManager::enable_music(bool enable) {
#ifdef HAVE_OPENAL
  if (device == nullptr)
    return;
#endif

  music_enabled = enable;
  if (music_enabled) {
    play_music(current_music);
  } else {
    stop_music();
  }
}

void SoundManager::stop_music(float fadetime) {
#ifdef HAVE_OPENAL
  if (fadetime > 0) {
    if (music_source &&
        music_source->get_fade_state() != StreamSoundSource::FadingOff)
      music_source->set_fading(StreamSoundSource::FadingOff, fadetime);
  } else {
    music_source.reset();
  }
#endif

#ifdef USE_SDL_MIXER
  if (fadetime > 0) {
    if (Mix_PlayingMusic() && Mix_FadingMusic() != MIX_FADING_OUT) {
      Mix_FadeOutMusic(static_cast<int>(fadetime * 1000));
    }
  } else {
    Mix_HaltMusic();

    if (current_music_resource) {
      current_music_resource->refcount--;
      if (current_music_resource->refcount <= 0) {
        free_music(current_music_resource);
      }
      current_music_resource = nullptr;
    }
  }
#endif

  current_music = "";
}

void SoundManager::play_music(const std::string &filename, bool fade) {
  if (filename == current_music) {
#ifdef HAVE_OPENAL
    if (music_source != nullptr) {
      if (music_source->paused()) {
        music_source->resume();
      } else if (!music_source->playing()) {
        music_source->play();
      }
      return;
    }
#endif
#ifdef USE_SDL_MIXER
    if (Mix_PlayingMusic()) {
      if (Mix_PausedMusic())
        Mix_ResumeMusic();
      return;
    }
#endif
  }

  current_music = filename;
  if (!music_enabled)
    return;

  if (filename.empty()) {
    stop_music();
    return;
  }

#ifdef HAVE_OPENAL
  try {
    std::unique_ptr<StreamSoundSource> newmusic(new StreamSoundSource());
    newmusic->set_sound_file(load_sound_file(filename));
    newmusic->set_looping(true);
    newmusic->set_relative(true);
    if (fade)
      newmusic->set_fading(StreamSoundSource::FadingOn, 0.5f);
    newmusic->play();

    music_source = std::move(newmusic);
  } catch (std::exception &e) {
    log_warning << "Couldn't play music file '" << filename << "': " << e.what()
                << std::endl;
    stop_music(0);
  }
#endif

#ifdef USE_SDL_MIXER
  // Check cache first
  auto it = musics.find(filename);

  if (it != musics.end() && it->second.music != nullptr) {
    if (current_music_resource && current_music_resource != &(it->second)) {
      current_music_resource->refcount--;
    }

    current_music_resource = &(it->second);
    current_music_resource->refcount++;

    Mix_HaltMusic();

    if (fade) {
      Mix_FadeInMusic(current_music_resource->music, -1, 500);
    } else {
      Mix_PlayMusic(current_music_resource->music, -1);
    }
    return;
  }

  // Not in cache, prepare to load
  if (current_music_resource) {
    current_music_resource->refcount--;
    current_music_resource = nullptr;
  }

  std::string load_filename = filename;
  if (load_filename.length() > 6) {
    std::string ext = load_filename.substr(load_filename.length() - 6);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".music") {
      load_filename.replace(load_filename.length() - 6, 6, ".ogg");
    }
  }

  try {
    Mix_Music *song = Mix_LoadMUS_RW(get_physfs_SDLRWops(load_filename), 1);
    if (!song) {
      log_warning << "Couldn't load music '" << load_filename
                  << "': " << Mix_GetError() << std::endl;
      return;
    }

    auto result = musics.insert(std::make_pair(filename, MusicResource()));
    current_music_resource = &(result.first->second);
    current_music_resource->manager = this;
    current_music_resource->music = song;
    current_music_resource->refcount = 1;

    Mix_HaltMusic();
    if (fade) {
      Mix_FadeInMusic(current_music_resource->music, -1, 500);
    } else {
      Mix_PlayMusic(current_music_resource->music, -1);
    }
  } catch (std::exception &e) {
    log_warning << "Couldn't load music '" << load_filename << "': " << e.what()
                << std::endl;
    return;
  }
#endif
}

void SoundManager::pause_music(float fadetime) {
#ifdef HAVE_OPENAL
  if (music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (music_source &&
        music_source->get_fade_state() != StreamSoundSource::FadingPause)
      music_source->set_fading(StreamSoundSource::FadingPause, fadetime);
  } else {
    music_source->pause();
  }
#endif

#ifdef USE_SDL_MIXER
  if (fadetime > 0) {
    Mix_FadeOutMusic(static_cast<int>(fadetime * 1000));
  } else {
    Mix_PauseMusic();
  }
#endif
}

void SoundManager::pause_sounds() {
#ifdef HAVE_OPENAL
  for (auto &source : sources) {
    if (auto *openal_source = dynamic_cast<OpenALSoundSource *>(source.get())) {
      if (openal_source->playing()) {
        openal_source->pause();
      }
    }
  }
#endif
#ifdef USE_SDL_MIXER
  Mix_Pause(-1);
#endif
}

void SoundManager::resume_sounds() {
#ifdef HAVE_OPENAL
  for (auto &source : sources) {
    if (auto *openal_source = dynamic_cast<OpenALSoundSource *>(source.get())) {
      if (openal_source->paused()) {
        openal_source->resume();
      }
    }
  }
#endif
#ifdef USE_SDL_MIXER
  Mix_Resume(-1);
#endif
}

void SoundManager::stop_sounds() {
  for (auto &source : sources) {
    source->stop();
  }
}

void SoundManager::resume_music(float fadetime) {
#ifdef HAVE_OPENAL
  if (music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (music_source &&
        music_source->get_fade_state() != StreamSoundSource::FadingResume)
      music_source->set_fading(StreamSoundSource::FadingResume, fadetime);
  } else {
    music_source->resume();
  }
#endif

#ifdef USE_SDL_MIXER
  if (fadetime > 0) {
    if (current_music_resource) {
      Mix_FadeInMusic(current_music_resource->music, -1,
                      static_cast<int>(fadetime * 1000));
    }
  } else {
    Mix_ResumeMusic();
  }
#endif
}

void SoundManager::set_listener_position(const Vector &pos) {
#ifdef HAVE_OPENAL
  static Uint32 lastticks = SDL_GetTicks();

  Uint32 current_ticks = SDL_GetTicks();
  if (current_ticks - lastticks < 300)
    return;
  lastticks = current_ticks;

  alListener3f(AL_POSITION, pos.x, pos.y, -300);
#endif
}

void SoundManager::set_listener_velocity(const Vector &vel) {
#ifdef HAVE_OPENAL
  alListener3f(AL_VELOCITY, vel.x, vel.y, 0);
#endif
}

void SoundManager::set_listener_orientation(const Vector &at,
                                            const Vector &up) {
#ifdef HAVE_OPENAL
  ALfloat orientation[] = {at.x, at.y, 1.0, up.x, up.y, 0.0};
  alListenerfv(AL_ORIENTATION, orientation);
#endif
}

void SoundManager::update() {
  static Uint32 lasttime = SDL_GetTicks();
  Uint32 now = SDL_GetTicks();

  if (now - lasttime < 300)
    return;
  lasttime = now;

  sources.erase(std::remove_if(sources.begin(), sources.end(),
                               [](const std::unique_ptr<SoundSource> &source) {
                                 return !source->playing();
                               }),
                sources.end());

#ifdef HAVE_OPENAL
  if (music_source) {
    music_source->update();
  }

  if (context) {
    alcProcessContext(context);
    check_alc_error("Error while processing audio context: ");
  }

  for (auto *sss : update_list) {
    sss->update();
  }
#endif
}

bool SoundManager::is_audio_enabled() const {
#ifdef HAVE_OPENAL
  return device != 0 && context != 0;
#endif
#ifdef USE_SDL_MIXER
  return sound_enabled;
#endif
  return false;
}

#ifdef USE_SDL_MIXER
void SoundManager::free_music(MusicResource *res) {
  if (res && res->music) {
    Mix_FreeMusic(res->music);
    res->music = nullptr;
  }
}

void SoundManager::clear_music_cache() {
  for (auto it = musics.begin(); it != musics.end();) {
    if (it->second.refcount <= 0) {
      if (it->second.music) {
        Mix_FreeMusic(it->second.music);
        it->second.music = nullptr;
      }
      it = musics.erase(it);
    } else {
      ++it;
    }
  }
}

void SoundManager::clear_sound_cache() {
  // Stop all playing sound effects first
  Mix_HaltChannel(-1);

  // Free all cached sound chunks
  for (auto &chunk_pair : sound_chunks) {
    Mix_FreeChunk(chunk_pair.second);
  }
  size_t count = sound_chunks.size();
  sound_chunks.clear();

  log_info << "Cleared " << count << " cached sounds" << std::endl;
}
#endif

#ifdef HAVE_OPENAL
ALenum SoundManager::get_sample_format(const SoundFile &file) {
  if (file.channels == 2) {
    if (file.bits_per_sample == 16) {
      return AL_FORMAT_STEREO16;
    } else if (file.bits_per_sample == 8) {
      return AL_FORMAT_STEREO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  } else if (file.channels == 1) {
    if (file.bits_per_sample == 16) {
      return AL_FORMAT_MONO16;
    } else if (file.bits_per_sample == 8) {
      return AL_FORMAT_MONO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  }

  throw std::runtime_error("Only 1 and 2 channel samples supported");
}

void SoundManager::print_openal_version() {
  log_info << "OpenAL Vendor: " << alGetString(AL_VENDOR) << std::endl;
  log_info << "OpenAL Version: " << alGetString(AL_VERSION) << std::endl;
  log_info << "OpenAL Renderer: " << alGetString(AL_RENDERER) << std::endl;
  log_info << "OpenAl Extensions: " << alGetString(AL_EXTENSIONS) << std::endl;
}

void SoundManager::check_alc_error(const char *message) const {
  int err = alcGetError(device);
  if (err != ALC_NO_ERROR) {
    std::stringstream msg;
    msg << message << alcGetString(device, err);
    throw std::runtime_error(msg.str());
  }
}

void SoundManager::check_al_error(const char *message) {
  int err = alGetError();
  if (err != AL_NO_ERROR) {
    std::stringstream msg;
    msg << message << alGetString(err);
    throw std::runtime_error(msg.str());
  }
}
#endif

// EOF
