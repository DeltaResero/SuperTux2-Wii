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
#include <ogc/usbstorage.h>
#include <sdcard/wiisd_io.h>
#include <wiiuse/wpad.h>

namespace Wii {

namespace {

/* Pages of cache, and sectors per page, handed to libfat for each device. */
const int CACHE_PAGES = 32;
const int SECTORS_PER_PAGE = 64;

const char* const APP_SUBDIR = "apps/supertux2-wii";

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

} // namespace Wii

#endif

/* EOF */
