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

#include "gui/menu_manager.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

ItemStringSelect::ItemStringSelect(const std::string& text, const std::vector<std::string>& list_, int* selected_, int id) :
  MenuItem(text, id),
  list(list_),
  selected(selected_),
  m_callback(),
  m_value_column(0.0f),
  m_pointer_x(),
  m_left_hover(0.0f),
  m_right_hover(0.0f),
  m_last_draw_time(0.0f)
{
}

namespace {

// Eases from the menu's own colour to its highlight colour as hover goes from 0 to 1
void draw_arrow(DrawingContext& context, const SurfacePtr& arrow, const Vector& pos, float hover)
{
  const Color& tint = ColorScheme::Menu::active_color;
  const Color color(1.0f + (tint.red - 1.0f) * hover,
                    1.0f + (tint.green - 1.0f) * hover,
                    1.0f + (tint.blue - 1.0f) * hover);
  context.color().draw_surface(arrow, pos, 0.0f, color, Blend(), LAYER_GUI);
}

} // namespace

void
ItemStringSelect::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active) {
  float roff = static_cast<float>(Resources::arrow_left->get_width()) * 1.0f;
  // Draw left side
  context.color().draw_text(Resources::normal_font, get_text(),
                              Vector(pos.x + 16.0f,
                                     pos.y - Resources::normal_font->get_height() / 2.0f),
                              ALIGN_LEFT, LAYER_GUI, active ? ColorScheme::Menu::active_color : get_color());

  // The arrow a click would use eases into the highlight colour and back
  const MenuAction hovered = m_pointer_x ? get_click_action(*m_pointer_x, menu_width) : MenuAction::NONE;
  const float dt = std::clamp(g_real_time - m_last_draw_time, 0.0f, 0.1f);
  m_last_draw_time = g_real_time;
  const float ease = std::min(1.0f, dt * 15.0f);
  m_left_hover += ((hovered == MenuAction::LEFT ? 1.0f : 0.0f) - m_left_hover) * ease;
  m_right_hover += ((hovered == MenuAction::RIGHT ? 1.0f : 0.0f) - m_right_hover) * ease;

  // Draw right side
  draw_arrow(context, Resources::arrow_left,
             Vector(pos.x + get_left_arrow_x(menu_width), pos.y - 8.0f), m_left_hover);
  draw_arrow(context, Resources::arrow_right,
             Vector(pos.x + static_cast<float>(menu_width) - roff - 8.0f, pos.y - 8.0f), m_right_hover);
  context.color().draw_text(Resources::normal_font, list[*selected],
                            Vector(pos.x + get_left_arrow_x(menu_width) + roff + get_column_width() / 2.0f,
                                   pos.y - Resources::normal_font->get_height() / 2.0f),
                            ALIGN_CENTER, LAYER_GUI, active ? ColorScheme::Menu::active_color : get_color());
}

float
ItemStringSelect::get_value_width() const {
  float width = 0.0f;
  for (const auto& value : list) {
    width = std::max(width, Resources::normal_font->get_text_width(value));
  }
  return width;
}

void
ItemStringSelect::set_value_column(float width) {
  m_value_column = width;
}

void
ItemStringSelect::set_pointer_x(const std::optional<float>& x) {
  m_pointer_x = x;
}

float
ItemStringSelect::get_column_width() const {
  // The menu's shared column, or this item's own widest value outside a menu
  return std::max(m_value_column, get_value_width());
}

float
ItemStringSelect::get_left_arrow_x(int menu_width) const {
  const float roff = static_cast<float>(Resources::arrow_left->get_width());
  return static_cast<float>(menu_width) - get_column_width() - 2.0f * roff - 8.0f;
}

MenuAction
ItemStringSelect::get_click_action(float x, int menu_width) const {
  const float roff = static_cast<float>(Resources::arrow_left->get_width());
  const float left_arrow_x = get_left_arrow_x(menu_width);
  const float middle = left_arrow_x + roff + get_column_width() / 2.0f;
  // An arrow's width before the left arrow still counts as the left arrow
  if (x >= left_arrow_x - roff && x < middle) {
    return MenuAction::LEFT;
  }
  if (x >= middle) {
    return MenuAction::RIGHT;
  }
  // The name has no direction to step in
  return MenuAction::NONE;
}

int
ItemStringSelect::get_width() const {
  return static_cast<int>(Resources::normal_font->get_text_width(get_text()) + get_column_width()) + 64;
}

void
ItemStringSelect::process_action(const MenuAction& action) {
  switch (action) {
    case MenuAction::LEFT:
      if ( (*selected) > 0) {
        (*selected)--;
      } else {
        (*selected) = static_cast<int>(list.size()) - 1;
      }
      MenuManager::instance().current_menu()->menu_action(*this);
      if (m_callback) {
        m_callback(*selected);
      }
      break;
    case MenuAction::RIGHT:
    case MenuAction::HIT:
      if ( (*selected)+1 < int(list.size())) {
        (*selected)++;
      } else {
        (*selected) = 0;
      }
      MenuManager::instance().current_menu()->menu_action(*this);
      if (m_callback) {
        m_callback(*selected);
      }
      break;
    default:
      break;
  }
}

/* EOF */
