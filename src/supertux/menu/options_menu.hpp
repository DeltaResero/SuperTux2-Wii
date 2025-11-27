// src/supertux/menu/options_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2004 Tobas Glaesser <tobi.web@gmx.de>
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_OPTIONS_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_OPTIONS_MENU_HPP

#include "gui/menu.hpp"

class OptionsMenu : public Menu
{
  public:
    OptionsMenu(bool complete);
    virtual ~OptionsMenu();

    void menu_action(MenuItem* item) override;

  private:
    int next_magnification;
    int next_aspect_ratio;
    int next_resolution;

    std::vector<std::string> magnifications;
    std::vector<std::string> aspect_ratios;
    std::vector<std::string> resolutions;
};

#endif // HEADER_SUPERTUX_SUPERTUX_OPTIONS_MENU_HPP

// EOF
