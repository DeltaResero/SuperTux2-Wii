// src/video/wii_texture_policy.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Texture Caching Policy for Low-End Systems
// Determines which textures should be kept in cache on memory-constrained
// systems

#ifndef HEADER_SUPERTUX_VIDEO_WII_TEXTURE_POLICY_HPP
#define HEADER_SUPERTUX_VIDEO_WII_TEXTURE_POLICY_HPP

#ifdef USE_SDL_MIXER

#include <string>

namespace WiiTexturePolicy {

/**
 * Determines if a texture should be kept in the texture cache.
 *
 * CACHE these (always needed):
 * - UI elements (menus, buttons)
 * - Fonts and text rendering
 * - Mouse cursor
 * - Tux (player character)
 * - Current level's tileset
 *
 * DON'T CACHE these (load on-demand):
 * - Level preview thumbnails
 * - Unused enemy sprites
 * - Background images for other levels
 * - Particle effect textures
 * - World map decorations
 *
 * @param filename The normalized file path of the texture
 * @return true if texture should be cached, false if load-on-demand
 */
inline bool should_cache_texture(const std::string &filename) {
  // === ALWAYS CACHE: UI ELEMENTS ===
  if (filename.find("images/engine/menu/") != std::string::npos) {
    return true;
  }

  if (filename.find("images/engine/fonts/") != std::string::npos) {
    return true;
  }

  if (filename.find("mousecursor") != std::string::npos) {
    return true;
  }

  if (filename.find("images/engine/editor/") != std::string::npos) {
    return true;
  }

  // === ALWAYS CACHE: PLAYER CHARACTER ===
  if (filename.find("images/creatures/tux/") != std::string::npos) {
    return true;
  }

  // === ALWAYS CACHE: TILES ===
  if (filename.find("images/tiles/") != std::string::npos) {
    return true;
  }

  // === DON'T CACHE: LEVEL PREVIEWS ===
  if (filename.find("images/worldmap/") != std::string::npos) {
    return false;
  }

  // === DON'T CACHE: MOST ENEMIES ===
  if (filename.find("images/creatures/") != std::string::npos) {
    return false;
  }

  // === DON'T CACHE: PARTICLES ===
  if (filename.find("images/particles/") != std::string::npos) {
    return false;
  }

  // === DON'T CACHE: BACKGROUNDS ===
  if (filename.find("images/background/") != std::string::npos) {
    return false;
  }

  // === DON'T CACHE: OBJECTS ===
  if (filename.find("images/objects/") != std::string::npos) {
    return false;
  }

  // === DEFAULT: DON'T CACHE ===
  return false;
}

} // namespace WiiTexturePolicy

#endif // USE_SDL_MIXER
#endif // HEADER_SUPERTUX_VIDEO_WII_TEXTURE_POLICY_HPP
