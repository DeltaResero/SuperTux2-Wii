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

#ifndef HEADER_SUPERTUX_AUDIO_FLAC_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_FLAC_SOUND_FILE_HPP

#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

#include <FLAC/stream_decoder.h>

#include "audio/sound_file.hpp"

class FlacSoundFile final : public SoundFile
{
public:
  FlacSoundFile(std::unique_ptr<std::istream> file);
  ~FlacSoundFile() override;

  virtual size_t read(void* buffer, size_t buffer_size) override;
  virtual void reset() override;

private:
  static FLAC__StreamDecoderReadStatus cb_read(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[],
                                               size_t* bytes, void* source);
  static FLAC__StreamDecoderSeekStatus cb_seek(const FLAC__StreamDecoder* decoder, FLAC__uint64 offset,
                                               void* source);
  static FLAC__StreamDecoderTellStatus cb_tell(const FLAC__StreamDecoder* decoder, FLAC__uint64* offset,
                                               void* source);
  static FLAC__StreamDecoderLengthStatus cb_length(const FLAC__StreamDecoder* decoder, FLAC__uint64* length,
                                                   void* source);
  static FLAC__bool cb_eof(const FLAC__StreamDecoder* decoder, void* source);
  static FLAC__StreamDecoderWriteStatus cb_write(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame,
                                                 const FLAC__int32* const samples[], void* source);
  static void cb_metadata(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata,
                          void* source);
  static void cb_error(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status,
                       void* source);

private:
  std::unique_ptr<std::istream> m_file;
  FLAC__StreamDecoder* m_decoder;

  /** The last decoded frame, and how many bytes of it read() has handed out */
  std::vector<int16_t> m_decoded;
  size_t m_decoded_offset;

private:
  FlacSoundFile(const FlacSoundFile&) = delete;
  FlacSoundFile& operator=(const FlacSoundFile&) = delete;
};

#endif

/* EOF */
