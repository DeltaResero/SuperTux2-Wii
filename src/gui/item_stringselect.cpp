// src/gui/item_stringselect.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "gui/item_stringselect.hpp"

#include <algorithm>
#include <stdio.h>

#include "gui/menu_action.hpp"
#include "gui/menu_manager.hpp"
#include "math/vector.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/resources.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

ItemStringSelect::ItemStringSelect(const std::string& text_, const std::vector<std::string>& list_, int* selected_, int _id) :
  MenuItem(text_, _id),
  list(list_),
  selected(selected_),
  widest_value(0.0f)
{
  measure_list();
}

void
ItemStringSelect::set_list(const std::vector<std::string>& new_list,
                           int* new_selected)
{
  list = new_list;
  selected = new_selected;
  measure_list();
}

void
ItemStringSelect::measure_list()
{
  /* Room is kept for the longest value the setting can take, so that nothing
     moves as it is stepped through. */
  widest_value = 0.0f;
  for(const auto& value : list)
  {
    widest_value = std::max(Resources::normal_font->get_text_width(value),
                            widest_value);
  }
}

namespace {

/** Where the arrows and the value column sit, measured from the item's left
    edge. Every setting in a menu is given the same column, so the arrows line
    up down the menu however long each value happens to be. */
void value_column(int menu_width, float value_width,
                  float& out_left_arrow, float& out_right_arrow,
                  float& out_centre)
{
  const float roff = Resources::arrow_left->get_width();
  out_right_arrow = menu_width - roff - 8;
  out_left_arrow  = out_right_arrow - value_width - roff;
  out_centre      = (out_left_arrow + roff + out_right_arrow) / 2;
}

} // namespace

void
ItemStringSelect::draw(DrawingContext& context, Vector pos, int menu_width, float value_width, bool active) {
  float left_arrow, right_arrow, centre;
  value_column(menu_width, value_width, left_arrow, right_arrow, centre);

  // Draw left side
  context.draw_text(Resources::normal_font, text,
                    Vector(pos.x + 16, pos.y - int(Resources::normal_font->get_height()/2)),
                    ALIGN_LEFT, LAYER_GUI, active ? ColorScheme::Menu::active_color : get_color());

  // Draw right side
  context.draw_surface(Resources::arrow_left,
                       Vector(pos.x + left_arrow, pos.y - 8),
                       LAYER_GUI);
  context.draw_surface(Resources::arrow_right,
                       Vector(pos.x + right_arrow, pos.y - 8),
                       LAYER_GUI);
  /* Centred rather than pushed against one arrow, so a short value does not
     sit at one end of the column with a hole beside it. */
  context.draw_text(Resources::normal_font, list[*selected],
                    Vector(pos.x + centre, pos.y - int(Resources::normal_font->get_height()/2)),
                    ALIGN_CENTER, LAYER_GUI, active ? ColorScheme::Menu::active_color : get_color());
}

int
ItemStringSelect::get_width() const {
  return Resources::normal_font->get_text_width(text) + static_cast<int>(widest_value) + 64;
}

float
ItemStringSelect::get_value_width() const {
  return widest_value;
}

MenuAction
ItemStringSelect::get_click_action(float x_offset, int menu_width,
                                   float value_width) const {
  /* The arrows are small and a press rarely lands square on one, so the whole
     column they sit either side of is split down the middle: the near half
     steps back and the far half steps forward. */
  float left_arrow, right_arrow, centre;
  value_column(menu_width, value_width, left_arrow, right_arrow, centre);

  const float roff = Resources::arrow_left->get_width();
  if(x_offset < left_arrow || x_offset >= right_arrow + roff)
    return MENU_ACTION_NONE;

  return (x_offset < centre) ? MENU_ACTION_LEFT : MENU_ACTION_RIGHT;
}

void
ItemStringSelect::process_action(MenuAction action) {
  switch (action) {
    case MENU_ACTION_LEFT:
      if( (*selected) > 0) {
        (*selected)--;
      } else {
        (*selected) = list.size()-1;
      }
      MenuManager::instance().current_menu()->menu_action(this);
      break;
    case MENU_ACTION_RIGHT:
      if( (*selected)+1 < int(list.size())) {
        (*selected)++;
      } else {
        (*selected) = 0;
      }
      MenuManager::instance().current_menu()->menu_action(this);
      break;
    default:
      break;
  }
}

/* EOF */
