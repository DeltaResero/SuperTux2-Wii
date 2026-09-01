// src/video/texture_manager.hpp
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

#ifndef HEADER_SUPERTUX_VIDEO_TEXTURE_MANAGER_HPP
#define HEADER_SUPERTUX_VIDEO_TEXTURE_MANAGER_HPP

#include <SDL_video.h>

#include <config.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "util/currenton.hpp"
#include "video/glutil.hpp"
#include "video/texture_ptr.hpp"

class Texture;
class Rect;

class TextureManager : public Currenton<TextureManager>
{
public:
  TextureManager();
  ~TextureManager();

  /** The whole picture as one texture, or nothing at all when this device
      will not take a texture that large. A caller handed nothing asks
      get_cells() how the picture has to be broken up instead. */
  TexturePtr get(const std::string& filename);
  TexturePtr get(const std::string& filename, const Rect& rect);

  /** The pieces a picture too large to take in one go has to be cut into,
      in reading order. Only meaningful straight after get() has declined
      the picture, since it answers from the copy that call left decoded. */
  std::vector<Rect> get_cells(const std::string& filename);

  /** Let go of the decoded copy of a picture. The pieces are cut from one
      shared copy, so the last cut releases it. */
  void release_image(const std::string& filename);

private:
  friend class Texture;

  typedef std::map<std::string, std::weak_ptr<Texture> > ImageTextures;
  ImageTextures m_image_textures;

  typedef std::map<std::string, SDL_Surface*> Surfaces;
  Surfaces m_surfaces;

private:
  void reap_cache_entry(const std::string& filename);

  TexturePtr create_image_texture(const std::string& filename, const Rect& rect);

  /** on failure a dummy texture is returned and no exception is thrown */
  TexturePtr create_image_texture(const std::string& filename);

  /** throw an exception on error */
  TexturePtr create_image_texture_raw(const std::string& filename);
  TexturePtr create_image_texture_raw(const std::string& filename, const Rect& rect);

  /** Decode a picture, or hand back the copy already decoded. Throws when
      the file cannot be read. */
  SDL_Surface* load_image(const std::string& filename);

  TexturePtr create_dummy_texture();

};

#endif

/* EOF */
