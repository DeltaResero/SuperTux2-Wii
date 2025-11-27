// src/supertux/menu/download_dialog.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_DOWNLOAD_DIALOG_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_DOWNLOAD_DIALOG_HPP

#include "addon/addon_manager.hpp"
#include "gui/dialog.hpp"

class TransferStatus;
using TransferStatusPtr = std::shared_ptr<TransferStatus>;

class DownloadDialog : public Dialog
{
private:
  TransferStatusPtr m_status;
  std::string m_title;
  bool m_auto_close;

public:
  DownloadDialog(TransferStatusPtr status, bool auto_close = false, bool passive = false);

  void set_title(const std::string& title);
  void update() override;

private:
  void on_abort();
  void on_download_complete();

  void update_text();

private:
  DownloadDialog(const DownloadDialog&) = delete;
  DownloadDialog& operator=(const DownloadDialog&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_DOWNLOAD_DIALOG_HPP

// EOF
