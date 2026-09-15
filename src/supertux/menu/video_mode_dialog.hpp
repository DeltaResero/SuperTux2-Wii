// src/supertux/menu/video_mode_dialog.hpp
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_VIDEO_MODE_DIALOG_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_VIDEO_MODE_DIALOG_HPP

#include "gui/dialog.hpp"
#include "math/size.hpp"

/** The video settings as a set, so a set can be put back. */
struct VideoSetting final
{
  VideoSetting() :
    fullscreen_size(0, 0),
    refresh_rate(0),
    window_size(0, 0),
    maximised(false)
  {}

  Size fullscreen_size;
  int refresh_rate;
  Size window_size;
  bool maximised;

  static VideoSetting current();
  void apply() const;

  bool operator==(const VideoSetting& other) const;
  bool operator!=(const VideoSetting& other) const { return !(*this == other); }
};

/** Puts a setting back unless it is kept in time.

    A screen handed a mode it cannot show displays nothing, so whoever most
    needs this cannot read it. Doing nothing is what undoes the change. */
class VideoModeDialog final : public Dialog
{
public:
  /** Asks about the setting in effect. Call once it has been applied. */
  static void ask(const VideoSetting& previous);

  void update() override;

private:
  explicit VideoModeDialog(const VideoSetting& previous);

  VideoSetting m_previous;
  float m_deadline;
  int m_counted;

private:
  VideoModeDialog(const VideoModeDialog&) = delete;
  VideoModeDialog& operator=(const VideoModeDialog&) = delete;
};

#endif

/* EOF */
