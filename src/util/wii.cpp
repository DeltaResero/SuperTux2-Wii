// src/util/wii.cpp
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

#ifdef __wii__

#include "util/wii.hpp"

#include <dirent.h>
#include <stdexcept>

#include <fat.h>
#include <ogc/system.h>
#include <ogc/usbstorage.h>
#include <ogc/video.h>
#include <sdcard/wiisd_io.h>
#include <wiiuse/wpad.h>

/* SDL's event pump sends SDL_QUIT while this is set. */
extern "C" bool OGC_ResetRequested;

namespace Wii {

namespace {

/* Pages of cache, and sectors per page, handed to libfat for each device. */
const int CACHE_PAGES = 32;
const int SECTORS_PER_PAGE = 64;

const char* const APP_SUBDIR = "apps/supertux2-wii";

/* Wii Linux's NTSC height, clear of the overscan most TVs crop. */
const u16 SAFE_HEIGHT = 448;

/* The same share of PAL's 576 lines. GX's copy stretch lands on exactly
   this, and the even origin keeps the fields in order. */
const u16 PAL_SAFE_LINES = 540;

/* Set from an interrupt. */
volatile bool power_off_requested = false;

bool sd_mounted = false;
bool usb_mounted = false;
std::string base_dir;

/* A released device has to drop the cached path naming it. */
void forget_base_dir(const std::string& device)
{
  if (base_dir.compare(0, device.size(), device) == 0)
    base_dir.clear();
}

bool dir_exists(const std::string& directory)
{
  DIR* dir = opendir(directory.c_str());
  if (dir == nullptr)
    return false;

  closedir(dir);
  return true;
}

void fit_safe_height(GXRModeObj& mode, u16 lines, u16 max_lines)
{
  mode.efbHeight = SAFE_HEIGHT;
  mode.xfbHeight = lines;
  mode.viHeight = lines;
  mode.viYOrigin = (max_lines - lines) / 2;
}

void on_power_button()
{
  power_off_requested = true;
  OGC_ResetRequested = true;
}

void on_remote_power_button(s32)
{
  on_power_button();
}

} // namespace

bool mount_sd()
{
  if (sd_mounted)
    return true;

  if (!__io_wiisd.startup())
    return false;

  /* A slot with no card in it starts up and then reports nothing inserted. */
  if (!__io_wiisd.isInserted())
  {
    __io_wiisd.shutdown();
    return false;
  }

  sd_mounted = fatMount("sd", &__io_wiisd, 0, CACHE_PAGES, SECTORS_PER_PAGE);
  if (!sd_mounted)
    __io_wiisd.shutdown();

  return sd_mounted;
}

void unmount_sd()
{
  if (!sd_mounted)
    return;

  fatUnmount("sd");
  __io_wiisd.shutdown();
  sd_mounted = false;
  forget_base_dir("sd:/");
}

bool mount_usb()
{
  if (usb_mounted)
    return true;

  __io_usbstorage.startup();
  usb_mounted = fatMount("usb", &__io_usbstorage, 0, CACHE_PAGES, SECTORS_PER_PAGE);
  if (!usb_mounted)
    __io_usbstorage.shutdown();

  return usb_mounted;
}

void unmount_usb()
{
  if (!usb_mounted)
    return;

  fatUnmount("usb");
  __io_usbstorage.shutdown();
  usb_mounted = false;
  forget_base_dir("usb:/");
}

std::string get_base_dir()
{
  if (!base_dir.empty())
    return base_dir;

  /* USB first: it is the faster of the two where both are present. */
  if (mount_usb())
  {
    const std::string path = std::string("usb:/") + APP_SUBDIR + "/";
    if (dir_exists(path))
    {
      base_dir = path;
      return base_dir;
    }
    unmount_usb();
  }

  if (mount_sd())
  {
    const std::string path = std::string("sd:/") + APP_SUBDIR + "/";
    if (dir_exists(path))
    {
      base_dir = path;
      return base_dir;
    }
    unmount_sd();
  }

  throw std::runtime_error("No SuperTux installation found in usb:/"
                           + std::string(APP_SUBDIR) + "/ or sd:/"
                           + std::string(APP_SUBDIR) + "/");
}

std::string get_user_dir()
{
  return get_base_dir() + "userdata/";
}

bool has_expansion()
{
  /* WPAD_Probe answers from the same flag the handshake sets, so this stays
     false until libogc has finished talking to the expansion. */
  u32 expansion = WPAD_EXP_NONE;
  return WPAD_Probe(WPAD_CHAN_0, &expansion) == WPAD_ERR_NONE
         && expansion != WPAD_EXP_NONE;
}

std::string get_data_dir()
{
  const std::string path = get_base_dir() + "data/";

  if (!dir_exists(path))
    throw std::runtime_error("No data directory at " + path);

  return path;
}

void take_power_buttons()
{
  SYS_SetPowerCallback(on_power_button);
  WPAD_SetPowerButtonCallback(on_remote_power_button);
}

void power_off_if_requested()
{
  if (!power_off_requested)
    return;

  SYS_ResetSystem(SYS_POWEROFF, 0, 0);

  /* libogc returns if IOS hasn't cut the power yet, with IOS already gone. */
  while (power_off_requested)
  {
  }
}

void fit_overscan()
{
  /* SDL lists the 240-line mode, which the menu never offers, so it carries
     the full height as a second choice. */
  GXRModeObj* preferred = VIDEO_GetPreferredMode(nullptr);
  if (preferred == &TVNtsc480IntDf || preferred == &TVNtsc480Prog)
    TVNtsc240Ds = *preferred;
  else if (preferred == &TVEurgb60Hz480IntDf || preferred == &TVEurgb60Hz480Prog)
    TVEurgb60Hz240Ds = *preferred;
  else if (preferred == &TVMpal480IntDf || preferred == &TVMpal480Prog)
    TVMpal240Ds = *preferred;
  else if (preferred == &TVPal576IntDfScale || preferred == &TVPal576ProgScale)
    TVPal264Ds = TVPal576IntDfScale;

  /* SDL offers these by pointer, so the change reaches its mode list. */
  fit_safe_height(TVNtsc480IntDf, SAFE_HEIGHT, VI_MAX_HEIGHT_NTSC);
  fit_safe_height(TVNtsc480Prog, SAFE_HEIGHT, VI_MAX_HEIGHT_NTSC);
  fit_safe_height(TVEurgb60Hz480IntDf, SAFE_HEIGHT, VI_MAX_HEIGHT_EURGB60);
  fit_safe_height(TVEurgb60Hz480Prog, SAFE_HEIGHT, VI_MAX_HEIGHT_EURGB60);
  fit_safe_height(TVMpal480IntDf, SAFE_HEIGHT, VI_MAX_HEIGHT_MPAL);
  fit_safe_height(TVMpal480Prog, SAFE_HEIGHT, VI_MAX_HEIGHT_MPAL);
  /* PAL stretches the same picture to fill the taller screen. */
  fit_safe_height(TVPal576IntDfScale, PAL_SAFE_LINES, VI_MAX_HEIGHT_PAL);

  /* The Wii can't send progressive at 50 Hz, so these become duplicates
     of the interlaced mode and SDL drops them. */
  TVPal528Prog = TVPal576IntDfScale;
  TVPal576ProgScale = TVPal576IntDfScale;
}

} // namespace Wii

#endif

/* EOF */
