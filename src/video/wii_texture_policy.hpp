// src/video/wii_texture_policy.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Wii Texture Caching Policy
// Determines which textures should be kept in cache on memory-constrained Wii

#ifndef HEADER_SUPERTUX_VIDEO_WII_TEXTURE_POLICY_HPP
#define HEADER_SUPERTUX_VIDEO_WII_TEXTURE_POLICY_HPP

#ifdef _WII_

#include <string>

namespace WiiTexturePolicy {

/**
 * Determines if a texture should be kept in the texture cache on Wii.
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
inline bool should_cache_texture(const std::string& filename)
{
  // === ALWAYS CACHE: UI ELEMENTS ===
  if (filename.find("images/engine/menu/") != std::string::npos) {
    return true;  // Menu buttons, checkboxes, arrows
  }

  if (filename.find("images/engine/fonts/") != std::string::npos) {
    return true;  // Font textures are small and always needed
  }

  if (filename.find("mousecursor") != std::string::npos) {
    return true;  // Tiny but always visible
  }

  if (filename.find("images/engine/editor/") != std::string::npos) {
    return true;  // Editor icons (if editor is enabled)
  }

  // === ALWAYS CACHE: PLAYER CHARACTER ===
  if (filename.find("images/creatures/tux/") != std::string::npos) {
    return true;  // Tux is in every level
  }

  // === ALWAYS CACHE: TILES ===
  // Note: This caches ALL tilesets, which may be too aggressive
  // Consider tracking "current level's tileset" if memory is still tight
  if (filename.find("images/tiles/") != std::string::npos) {
    return true;  // Tiles are reused constantly within a level
  }

  // === DON'T CACHE: LEVEL PREVIEWS ===
  if (filename.find("images/worldmap/") != std::string::npos) {
    return false;  // Large images only shown briefly
  }

  // === DON'T CACHE: MOST ENEMIES ===
  // Enemies are level-specific and should be loaded per-level
  if (filename.find("images/creatures/") != std::string::npos) {
    return false;  // Load on-demand per level
  }

  // === DON'T CACHE: PARTICLES ===
  if (filename.find("images/particles/") != std::string::npos) {
    return false;  // Small textures, not always visible
  }

  // === DON'T CACHE: BACKGROUNDS ===
  // Backgrounds are large and level-specific
  if (filename.find("images/background/") != std::string::npos) {
    return false;  // Load per-level
  }

  // === DON'T CACHE: OBJECTS ===
  // Most objects are level-specific
  if (filename.find("images/objects/") != std::string::npos) {
    return false;  // Load on-demand
  }

  // === DEFAULT: DON'T CACHE ===
  // Conservative approach - if we don't recognize it, don't cache it
  // This prevents runaway memory usage from unexpected assets
  return false;
}

} // namespace WiiTexturePolicy

#endif // _WII_
#endif // HEADER_SUPERTUX_VIDEO_WII_TEXTURE_POLICY_HPP
