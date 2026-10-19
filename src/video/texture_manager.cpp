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

#include "video/texture_manager.hpp"

#include <SDL_image.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <sstream>
#include <string_view>
#include <vector>

#include "math/rect.hpp"
#include "io/sdl_file.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "video/color.hpp"
#include "video/gl.hpp"
#include "video/sampler.hpp"
#include "video/sdl_surface.hpp"
#include "video/texture.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

GLenum string2wrap(const std::string& text)
{
  if (text == "clamp-to-edge")
  {
    return GL_CLAMP_TO_EDGE;
  }
  else if (text == "repeat")
  {
    return GL_REPEAT;
  }
  else if (text == "mirrored-repeat")
  {
    return GL_MIRRORED_REPEAT;
  }
  else
  {
    log_warning << "unknown texture wrap: " << text << std::endl;
    return GL_CLAMP_TO_EDGE;
  }
}

GLenum string2filter(const std::string& text)
{
  if (text == "nearest")
  {
    return GL_NEAREST;
  }
  else if (text == "linear")
  {
    return GL_LINEAR;
  }
  else
  {
    log_warning << "unknown texture filter: " << text << std::endl;
    return GL_LINEAR;
  }
}

// Menus, the HUD and fonts are drawn at their own size, not the level's
bool shrinks_with_view(const std::string& filename)
{
  const std::string_view engine = "images/engine/";
  const std::string::size_type start = filename.find_first_not_of('/');
  return start == std::string::npos || filename.compare(start, engine.size(), engine) != 0;
}

} // namespace

TextureManager::TextureManager() :
  m_image_textures(),
  m_surfaces(),
  m_resize(1.0f, 1.0f),
  m_pixels(),
  m_pixels_clock(0),
  m_generation(0)
{
}

TextureManager::~TextureManager()
{
  for (const auto& texture : m_image_textures)
  {
    if (!texture.second.expired())
    {
      log_warning << "Texture '" << std::get<0>(texture.first) << "' not freed" << std::endl;
    }
  }
  m_image_textures.clear();
  m_surfaces.clear();
}

TexturePtr
TextureManager::get(const ReaderMapping& mapping, const std::optional<Rect>& region)
{
  std::string filename;
  if (!mapping.get("file", filename))
  {
    log_warning << "'file' tag missing" << std::endl;
  }
  else
  {
    filename = FileSystem::join(mapping.get_doc().get_directory(), filename);
  }

  std::optional<Rect> rect;
  std::vector<int> rect_v;
  if (mapping.get("rect", rect_v))
  {
    if (rect_v.size() == 4)
    {
      rect = Rect(rect_v[0], rect_v[1], rect_v[2], rect_v[3]);
    }
    else
    {
      log_warning << "'rect' requires four elements" << std::endl;
    }
  }

  GLenum wrap_s = GL_CLAMP_TO_EDGE;
  GLenum wrap_t = GL_CLAMP_TO_EDGE;

  std::vector<std::string> wrap_v;
  if (mapping.get("wrap", wrap_v))
  {
    if (wrap_v.size() == 1)
    {
      wrap_s = string2wrap(wrap_v[0]);
      wrap_t = string2wrap(wrap_v[0]);
    }
    else if (wrap_v.size() == 2)
    {
      wrap_s = string2wrap(wrap_v[0]);
      wrap_t = string2wrap(wrap_v[1]);
    }
    else
    {
      log_warning << "unknown number of wrap arguments" << std::endl;
    }
  }

  GLenum filter = GL_LINEAR;
  std::string filter_s;
  if (mapping.get("filter", filter_s))
  {
    filter = string2filter(filter_s);
  }

  Vector animate(0.0f, 0.0f);
  std::vector<float> animate_v;
  if (mapping.get("animate", animate_v))
  {
    if (animate_v.size() == 2)
    {
      animate.x = animate_v[0];
      animate.y = animate_v[1];
    }
  }

  if (region)
  {
    if (!rect)
    {
      rect = region;
    }
    else
    {
      rect->left += region->left;
      rect->top += region->top;

      rect->right = rect->left + region->get_width();
      rect->bottom = rect->top + region->get_height();
    }
  }

  return get(filename, rect, Sampler(filter, wrap_s, wrap_t, animate));
}

TexturePtr
TextureManager::get(const std::string& _filename)
{
  std::string filename = FileSystem::normalize(_filename);
  Texture::Key key(filename, Rect(0, 0, 0, 0));
  auto i = m_image_textures.find(key);

  TexturePtr texture;
  if (i != m_image_textures.end())
    texture = i->second.lock();

  if (!texture) {
    texture = create_image_texture(filename, Sampler());
    texture->m_cache_key = key;
    m_image_textures[key] = texture;
  }

  return texture;
}

TexturePtr
TextureManager::get(const std::string& _filename,
                    const std::optional<Rect>& rect,
                    const Sampler& sampler)
{
  std::string filename = FileSystem::normalize(_filename);
  Texture::Key key;
  if (rect)
  {
    key = Texture::Key(filename, *rect);
  }
  else
  {
    key = Texture::Key(filename, Rect());
  }

  auto i = m_image_textures.find(key);

  TexturePtr texture;
  if (i != m_image_textures.end())
    texture = i->second.lock();

  if (!texture) {
    if (rect)
    {
      texture = create_image_texture(filename, *rect, sampler);
    }
    else
    {
      texture = create_image_texture(filename, sampler);
    }
    texture->m_cache_key = key;
    m_image_textures[key] = texture;
  }

  return texture;
}

void
TextureManager::set_viewport(const Viewport& viewport, bool enlarge)
{
  // Shrinking once here keeps every row of a picture, where drawing it smaller drops rows unevenly
  const Vector& scale = viewport.get_scale();
  const Vector resize = enlarge ? scale : Vector(std::min(1.0f, scale.x), std::min(1.0f, scale.y));
  if (resize == m_resize)
  {
    return;
  }
  m_resize = resize;
  ++m_generation;
  m_pixels.clear();

  for (const auto& entry : m_image_textures)
  {
    const TexturePtr texture = entry.second.lock();
    const std::string& filename = std::get<0>(entry.first);
    if (!texture || !shrinks_with_view(filename))
    {
      continue;
    }

    try
    {
      const Rect& rect_in_file = std::get<1>(entry.first);
      if (rect_in_file.empty())
      {
        create_image_texture_raw(filename, Sampler(), texture);
      }
      else
      {
        create_image_texture_raw(filename, rect_in_file, Sampler(), texture);
      }
    }
    catch (const std::exception& err)
    {
      log_warning << "Couldn't reload texture '" << filename << "': " << err.what() << std::endl;
    }
  }
}

SDLSurfacePtr
TextureManager::prepare(const std::string& filename, const SDL_Surface& image) const
{
  SDLSurfacePtr own;
  const Vector shrink(std::min(1.0f, m_resize.x), std::min(1.0f, m_resize.y));
  if ((shrink.x < 1.0f || shrink.y < 1.0f) && shrinks_with_view(filename))
  {
    own = SDLSurface::shrink(image,
                             std::max(1, static_cast<int>(std::lround(static_cast<float>(image.w) * shrink.x))),
                             std::max(1, static_cast<int>(std::lround(static_cast<float>(image.h) * shrink.y))));
  }
  else if ((m_resize.x > 1.0f || m_resize.y > 1.0f) && shrinks_with_view(filename))
  {
    // Stretched as drawing would stretch it, in RGBA even when opaque as the software renderer blends that fastest
    SDLSurfacePtr source(SDL_ConvertSurfaceFormat(const_cast<SDL_Surface*>(&image), SDL_PIXELFORMAT_RGBA32, 0));
    if (!source)
    {
      std::ostringstream msg;
      msg << "failed to convert SDL_Surface: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
    if (SDL_ISPIXELFORMAT_ALPHA(image.format->format))
    {
      SDLSurface::fill_clear_pixels(*source);
    }
    // Rounded up, so the picture is never smaller than the whole pixels it's drawn into
    own = SDLSurface::create_rgba(static_cast<int>(std::ceil(static_cast<float>(image.w) * m_resize.x - 0.001f)),
                                  static_cast<int>(std::ceil(static_cast<float>(image.h) * m_resize.y - 0.001f)));
    if (SDL_SoftStretchLinear(source.get(), nullptr, own.get(), nullptr) != 0)
    {
      std::ostringstream msg;
      msg << "failed to enlarge SDL_Surface: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
    return own;
  }
  else if (SDL_ISPIXELFORMAT_ALPHA(image.format->format))
  {
    own.reset(SDL_ConvertSurfaceFormat(const_cast<SDL_Surface*>(&image), SDL_PIXELFORMAT_RGBA32, 0));
  }
  if (own)
  {
    SDLSurface::fill_clear_pixels(*own);
  }
  return own;
}

TexturePtr
TextureManager::make_texture(const std::string& filename, const SDL_Surface& image, const Sampler& sampler,
                             const TexturePtr& reload)
{
  SDLSurfacePtr own = prepare(filename, image);
  const SDL_Surface& pixels = own ? *own : image;

  TexturePtr texture = reload;
  if (texture)
  {
    texture->reload(pixels);
  }
  else
  {
    texture = VideoSystem::current()->new_texture(pixels, sampler, Size(image.w, image.h));
  }
  // Only tile-sized pictures are bordered at their joints, and their pixels are made again only when that needs them
  m_pixels.erase(texture.get());
  texture->m_pixel_size = (image.w == 32 && image.h == 32) ? Size(pixels.w, pixels.h) : Size(0, 0);
  return texture;
}

const SDL_Surface*
TextureManager::get_pixels(const Texture& texture)
{
  if (texture.m_pixel_size.width == 0 || !texture.m_cache_key)
  {
    return nullptr;
  }
  auto it = m_pixels.find(&texture);
  if (it == m_pixels.end())
  {
    const std::string& filename = std::get<0>(*texture.m_cache_key);
    const Rect& rect = std::get<1>(*texture.m_cache_key);
    SDLSurfacePtr rgba;
    try
    {
      SDLSurfacePtr keep;
      SDLSurfacePtr image = rect.empty() ? SDLSurface::from_file(filename) : cut(filename, rect, keep);
      if (!image) return nullptr;
      SDLSurfacePtr own = prepare(filename, *image);
      rgba.reset(SDL_ConvertSurfaceFormat(own ? own.get() : image.get(), SDL_PIXELFORMAT_RGBA32, 0));
    }
    catch (const std::exception& err)
    {
      log_warning << "Couldn't load the pixels of '" << filename << "': " << err.what() << std::endl;
    }
    if (!rgba) return nullptr;
    it = m_pixels.emplace(&texture, Pixels{ std::move(rgba), 0 }).first;
  }
  it->second.used = m_pixels_clock;
  return it->second.surface.get();
}

void
TextureManager::trim_pixels(size_t keep)
{
  ++m_pixels_clock;
  if (m_pixels.size() <= keep) return;
  std::vector<uint32_t> used;
  used.reserve(m_pixels.size());
  for (const auto& entry : m_pixels) used.push_back(entry.second.used);
  std::nth_element(used.begin(), used.begin() + static_cast<long>(keep), used.end(), std::greater<uint32_t>());
  const uint32_t oldest_kept = used[keep];
  for (auto it = m_pixels.begin(); it != m_pixels.end();)
  {
    it = (it->second.used <= oldest_kept) ? m_pixels.erase(it) : std::next(it);
  }
}

void
TextureManager::reap_cache_entry(const Texture::Key& key)
{
  auto i = m_image_textures.find(key);
  if (i == m_image_textures.end())
  {
    log_warning << "no cache entry for '" << std::get<0>(key) << "'" << std::endl;
  }
  else
  {
    assert(i->second.expired());
    m_image_textures.erase(i);
  }
}

TexturePtr
TextureManager::create_image_texture(const std::string& filename, const Rect& rect, const Sampler& sampler)
{
  try
  {
    return create_image_texture_raw(filename, rect, sampler);
  }
  catch(const std::exception& err)
  {
    log_warning << "Couldn't load texture '" << filename << "' (now using dummy texture): " << err.what() << std::endl;
    return create_dummy_texture();
  }
}

const SDL_Surface&
TextureManager::get_surface(const std::string& filename)
{
  auto i = m_surfaces.find(filename);
  if (i != m_surfaces.end())
  {
    return *i->second;
  }
  else
  {
    SDLSurfacePtr image = SDLSurface::from_file(filename);
    if (!image)
    {
      std::ostringstream msg;
      msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
      throw std::runtime_error(msg.str());
    }

    return *(m_surfaces[filename] = std::move(image));
  }
}

TexturePtr
TextureManager::create_image_texture_raw(const std::string& filename, const Rect& rect, const Sampler& sampler,
                                         const TexturePtr& reload)
{
  SDLSurfacePtr keep;
  SDLSurfacePtr subimage = cut(filename, rect, keep);
  return make_texture(filename, *subimage, sampler, reload);
}

SDLSurfacePtr
TextureManager::cut(const std::string& filename, const Rect& rect, SDLSurfacePtr& keep)
{
  assert(rect.valid());

  const SDL_Surface& src_surface = get_surface(filename);

  SDLSurfacePtr& convert = keep;
  if (src_surface.format->Rmask == 0 &&
      src_surface.format->Gmask == 0 &&
      src_surface.format->Bmask == 0 &&
      src_surface.format->Amask == 0)
  {
    log_debug << "Wrong surface format for image " << filename << ". Compensating." << std::endl;
    convert.reset(SDL_ConvertSurfaceFormat(const_cast<SDL_Surface*>(&src_surface), SDL_PIXELFORMAT_RGBA8888, 0));
  }

  const SDL_Surface& surface = convert ? *convert : src_surface;

  SDLSurfacePtr subimage;
  if (!Rect(0, 0, surface.w, surface.h).contains(rect))
  {
    log_warning << filename << ": invalid subregion requested: image="
                << surface.w << "x" << surface.h << ", rect=" << rect << std::endl;

    subimage = SDLSurfacePtr(SDL_CreateRGBSurface(0,
                                                  rect.get_width(),
                                                  rect.get_height(),
                                                  surface.format->BitsPerPixel,
                                                  surface.format->Rmask,
                                                  surface.format->Gmask,
                                                  surface.format->Bmask,
                                                  surface.format->Amask));

    Rect clipped_rect(std::max(0, rect.left),
                      std::max(0, rect.top),
                      std::min(subimage->w, rect.right),
                      std::min(subimage->w, rect.bottom));

    SDL_Rect srcrect = clipped_rect.to_sdl();
    SDL_BlitSurface(const_cast<SDL_Surface*>(&surface), &srcrect, subimage.get(), nullptr);
  }
  else
  {
    subimage = SDLSurfacePtr(SDL_CreateRGBSurfaceFrom(static_cast<uint8_t*>(surface.pixels) +
                                                      rect.top * surface.pitch +
                                                      rect.left * surface.format->BytesPerPixel,
                                                      rect.get_width(), rect.get_height(),
                                                      surface.format->BitsPerPixel,
                                                      surface.pitch,
                                                      surface.format->Rmask,
                                                      surface.format->Gmask,
                                                      surface.format->Bmask,
                                                      surface.format->Amask));
    if (!subimage)
    {
      throw std::runtime_error("SDL_CreateRGBSurfaceFrom() call failed");
    }
  }
  return subimage;
}

TexturePtr
TextureManager::create_image_texture(const std::string& filename, const Sampler& sampler)
{
  try
  {
    return create_image_texture_raw(filename, sampler);
  }
  catch (const std::exception& err)
  {
    log_warning << "Couldn't load texture '" << filename << "' (now using dummy texture): " << err.what() << std::endl;
    return create_dummy_texture();
  }
}

TexturePtr
TextureManager::create_image_texture_raw(const std::string& filename, const Sampler& sampler,
                                         const TexturePtr& reload)
{
  SDLSurfacePtr image = SDLSurface::from_file(filename);
  if (!image)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    return make_texture(filename, *image, sampler, reload);
  }
}

TexturePtr
TextureManager::create_dummy_texture()
{
  const std::string dummy_texture_fname = "images/engine/missing.png";

  // on error, try loading placeholder file
  try
  {
    TexturePtr tex = create_image_texture_raw(dummy_texture_fname, Sampler());
    return tex;
  }
  catch (const std::exception& err)
  {
    // on error (when loading placeholder), try using empty surface,
    // when that fails to, just give up
    SDLSurfacePtr image(SDL_CreateRGBSurface(0, 1024, 1024, 8, 0, 0, 0, 0));
    if (!image)
    {
      throw;
    }
    else
    {
      log_warning << "Couldn't load texture '" << dummy_texture_fname << "' (now using empty one): " << err.what() << std::endl;
      TexturePtr texture = VideoSystem::current()->new_texture(*image);
      return texture;
    }
  }
}

void
TextureManager::debug_print(std::ostream& out) const
{
  size_t total_texture_pixels = 0;
  out << "textures:begin" << std::endl;
  for(const auto& it : m_image_textures)
  {
    const auto& key = it.first;

    if (it.second.lock()) {
      total_texture_pixels += std::get<1>(key).get_area();
    }

    out << "  texture "
        << " filename:" << std::get<0>(key) << " " << std::get<1>(key)
        << " " << "use_count:" << it.second.use_count() << std::endl;
  }
  out << "textures:end" << std::endl;

  size_t total_surface_pixels = 0;
  out << "surfaces:begin" << std::endl;
  for(const auto& it : m_surfaces)
  {
    const auto& filename = it.first;
    const auto& surface = it.second;

    total_surface_pixels += surface->w * surface->h;
    out << "  surface filename:" << filename << " " << surface->w << "x" << surface->h << std::endl;
  }
  out << "surfaces:end" << std::endl;

  out << "total texture count:" << m_image_textures.size() << std::endl;
  out << "total texture pixels:" << total_texture_pixels << std::endl;

  out << "total surface count:" << m_surfaces.size() << std::endl;
  out << "total surface pixels:" << total_surface_pixels << std::endl;
}

/* EOF */
