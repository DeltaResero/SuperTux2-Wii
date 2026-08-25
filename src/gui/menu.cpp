// src/gui/menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include "gui/menu.hpp"
#include <numbers>

#include <algorithm>
#include <math.h>
#include <stdexcept>

#define INCLUDE_MENU_ITEMS
// This causes the #include "gui/menu_item.hpp" to include all menu items too.

#include "control/input_manager.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "gui/mousecursor.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/timer.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

static const float MENU_REPEAT_INITIAL = 0.4f;
static const float MENU_REPEAT_RATE    = 0.1f;

/** How far the panel drawn behind a menu reaches past the items themselves.
    Matches the rectangles MenuManager draws. */
static const float MENU_PANEL_EDGE = 14.0f;

/** Gap between the foot of that panel and the help box below it. */
static const float HELP_GAP = 12.0f;

/** Space between the help text and the edge of the box holding it. */
static const float HELP_PAD_Y = 4.0f;
static const float HELP_PAD_X = 8.0f;

/** How far the lighter box is drawn inside the darker one behind it. */
static const float HELP_BORDER = 4.0f;

/** How much taller the pair of boxes is than the text they hold. */
static const float HELP_BOX_PADDING = (HELP_PAD_Y + HELP_BORDER) * 2;

/** Room along the foot of the screen the title screen writes its notice in. */
static const float NOTICE_FOOT = 45.0f;

/** Clear space kept between the help box and that notice. */
static const float NOTICE_GAP = 4.0f;

Menu::Menu() :
  pos(),
  delete_character(),
  mn_input_char(),
  menu_repeat_time(),
  items(),
  arrange_left(),
  active_item()
{
  delete_character = 0;
  mn_input_char = '\0';

  pos.x        = SCREEN_WIDTH/2;
  pos.y        = SCREEN_HEIGHT/2;
  arrange_left = 0;
  active_item  = -1;
}

Menu::~Menu()
{
}

void
Menu::set_center_pos(float x, float y)
{
  pos.x = x;
  pos.y = y;
}

/* Add an item to a menu */
MenuItem*
Menu::add_item(std::unique_ptr<MenuItem> new_item)
{
  items.push_back(std::move(new_item));
  MenuItem* item = items.back().get();

  /* If a new menu is being built, the active item shouldn't be set to
   * something that isn't selectable. Set the active_item to the first
   * selectable item added.
   */

  if (active_item == -1 && !item->skippable())
  {
    active_item = items.size() - 1;
  }

  return item;
}

MenuItem*
Menu::add_item(std::unique_ptr<MenuItem> new_item, int pos_)
{
  items.insert(items.begin()+pos_,std::move(new_item));
  MenuItem* item = items[pos_].get();

  /* When the item is inserted before the selected item, the
   * same menu item should be still selected.
   */

  if (active_item >= pos_)
  {
    active_item++;
  }

  return item;
}

void
Menu::delete_item(int pos_)
{
  items.erase(items.begin()+pos_);

  /* When the item is deleted before the selected item, the
   * same menu item should be still selected.
   */

  if (active_item >= pos_)
  {
    do {
      if (active_item > 0)
        --active_item;
      else
        active_item = int(items.size())-1;
    } while (items[active_item]->skippable());
  }
}

MenuItem*
Menu::add_hl()
{
  std::unique_ptr<ItemHorizontalLine> item(new ItemHorizontalLine());
  return add_item(std::move(item));
}

MenuItem*
Menu::add_label(const std::string& text)
{
  std::unique_ptr<ItemLabel> item(new ItemLabel(text));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_controlfield(int id, const std::string& text,
                       const std::string& mapping)
{
  std::unique_ptr<ItemControlField> item(new ItemControlField(text, mapping, id));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_entry(int id, const std::string& text)
{
  std::unique_ptr<ItemAction> item(new ItemAction(text, id));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_inactive(const std::string& text)
{
  std::unique_ptr<ItemInactive> item(new ItemInactive(text));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_toggle(int id, const std::string& text, bool* toggled)
{
  std::unique_ptr<ItemToggle> item(new ItemToggle(text, toggled, id));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_string_select(int id, const std::string& text, int* selected, const std::vector<std::string>& strings)
{
  std::unique_ptr<ItemStringSelect> item(new ItemStringSelect(text, strings, selected, id));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_back(const std::string& text, int id)
{
  std::unique_ptr<ItemBack> item(new ItemBack(text, id));
  return add_item(std::move(item));
}

MenuItem*
Menu::add_submenu(const std::string& text, int submenu, int id)
{
  std::unique_ptr<ItemGoTo> item(new ItemGoTo(text, submenu, id));
  return add_item(std::move(item));
}

void
Menu::clear()
{
  items.clear();
  active_item = -1;
}

void
Menu::process_input()
{
  int menu_height = (int) get_height();
  if (menu_height > SCREEN_HEIGHT)
  { // Scrolling
    int scroll_offset = (menu_height - SCREEN_HEIGHT) / 2 + 32;
    pos.y = SCREEN_HEIGHT/2 - scroll_offset * ((float(active_item) / (items.size()-1)) - 0.5f) * 2.0f;
  }

  MenuAction menuaction = MENU_ACTION_NONE;
  auto controller = InputManager::current()->get_controller();
  /** check main input controller... */
  if(controller->pressed(Controller::UP)) {
    menuaction = MENU_ACTION_UP;
    menu_repeat_time = real_time + MENU_REPEAT_INITIAL;
  }
  if(controller->hold(Controller::UP) &&
     menu_repeat_time != 0 && real_time > menu_repeat_time) {
    menuaction = MENU_ACTION_UP;
    menu_repeat_time = real_time + MENU_REPEAT_RATE;
  }

  if(controller->pressed(Controller::DOWN)) {
    menuaction = MENU_ACTION_DOWN;
    menu_repeat_time = real_time + MENU_REPEAT_INITIAL;
  }
  if(controller->hold(Controller::DOWN) &&
     menu_repeat_time != 0 && real_time > menu_repeat_time) {
    menuaction = MENU_ACTION_DOWN;
    menu_repeat_time = real_time + MENU_REPEAT_RATE;
  }

  if(controller->pressed(Controller::LEFT)) {
    menuaction = MENU_ACTION_LEFT;
    menu_repeat_time = real_time + MENU_REPEAT_INITIAL;
  }
  if(controller->hold(Controller::LEFT) &&
     menu_repeat_time != 0 && real_time > menu_repeat_time) {
    menuaction = MENU_ACTION_LEFT;
    menu_repeat_time = real_time + MENU_REPEAT_RATE;
  }

  if(controller->pressed(Controller::RIGHT)) {
    menuaction = MENU_ACTION_RIGHT;
    menu_repeat_time = real_time + MENU_REPEAT_INITIAL;
  }
  if(controller->hold(Controller::RIGHT) &&
     menu_repeat_time != 0 && real_time > menu_repeat_time) {
    menuaction = MENU_ACTION_RIGHT;
    menu_repeat_time = real_time + MENU_REPEAT_RATE;
  }

  if(controller->pressed(Controller::ACTION)
     || controller->pressed(Controller::MENU_SELECT)
     || (!is_sensitive() && controller->pressed(Controller::MENU_SELECT_SPACE))) {
    menuaction = MENU_ACTION_HIT;
  }
  if(controller->pressed(Controller::ESCAPE) ||
     controller->pressed(Controller::CHEAT_MENU) ||
     controller->pressed(Controller::MENU_BACK)) {
    menuaction = MENU_ACTION_BACK;
  }

  if(controller->pressed(Controller::REMOVE)) {
    menuaction = MENU_ACTION_REMOVE;
    menu_repeat_time = real_time + MENU_REPEAT_INITIAL;
  }
  if(controller->hold(Controller::REMOVE) &&
     menu_repeat_time != 0 && real_time > menu_repeat_time) {
    menuaction = MENU_ACTION_REMOVE;
    menu_repeat_time = real_time + MENU_REPEAT_RATE;
  }

  if(items.size() == 0)
    return;

  // The menu_action() call can pop() the menu from the stack and thus
  // delete it, so it's important that no further member variables are
  // accessed after this call
  process_action(menuaction);
}

void
Menu::process_action(MenuAction menuaction)
{
  int last_active_item = active_item;

  switch(menuaction) {
    case MENU_ACTION_UP:
      do {
        if (active_item > 0)
          --active_item;
        else
          active_item = int(items.size())-1;
      } while (items[active_item]->skippable()
               && (active_item != last_active_item));
      break;

    case MENU_ACTION_DOWN:
      do {
        if(active_item < int(items.size())-1 )
          ++active_item;
        else
          active_item = 0;
      } while (items[active_item]->skippable()
               && (active_item != last_active_item));
      break;

    case MENU_ACTION_BACK:
      if(on_back_action()) {
        MenuManager::instance().pop_menu();
      }
      return;
      break;

    default:
      break;
  }

  if (items[active_item]->no_other_action()) {
    items[active_item]->process_action(menuaction);
    return;
  }

  items[active_item]->process_action(menuaction);
  if(menuaction == MENU_ACTION_HIT) {
    menu_action(items[active_item].get());
  }

}

void
Menu::draw_item(DrawingContext& context, int index)
{
  float menu_height = get_height();
  float menu_width  = get_width();

  MenuItem* pitem = items[index].get();

  float x_pos       = pos.x - menu_width/2;
  float y_pos       = pos.y + 24*index - menu_height/2 + 12;

  pitem->draw(context, Vector(x_pos, y_pos), menu_width, get_value_width(),
              active_item == index);

  if(active_item == index)
  {
    float blink = (sinf(real_time * std::numbers::pi_v<float> * 1.0f)/2.0f + 0.5f) * 0.5f + 0.25f;
    context.draw_filled_rect(Rectf(Vector(pos.x - menu_width/2 + 10 - 2, y_pos - 12 - 2),
                                   Vector(pos.x + menu_width/2 - 10 + 2, y_pos + 12 + 2)),
                             Color(1.0f, 1.0f, 1.0f, blink),
                             14.0f,
                             LAYER_GUI-10);
    context.draw_filled_rect(Rectf(Vector(pos.x - menu_width/2 + 10, y_pos - 12),
                                   Vector(pos.x + menu_width/2 - 10, y_pos + 12)),
                             Color(1.0f, 1.0f, 1.0f, 0.5f),
                             12.0f,
                             LAYER_GUI-10);
  }
}

float
Menu::get_width() const
{
  /* The width of the menu has to be more than the width of the text
     with the most characters */
  const float value_width = get_value_width();
  float menu_width = 0;
  for(unsigned int i = 0; i < items.size(); ++i)
  {
    float w = items[i]->get_width();

    /* An item asks for room for its own longest value, but it is given the
       column the whole menu shares, so make up the difference or a long name
       beside a short value would run into the arrow. */
    const float own_value = items[i]->get_value_width();
    if(own_value > 0)
      w += value_width - own_value;

    if(w > menu_width)
      menu_width = w;
  }

  return menu_width + 24;
}

float
Menu::get_value_width() const
{
  /* One column for the whole menu, wide enough for the longest value any of
     its settings can take, so the arrows sit in the same place on every row
     rather than wherever each row's own values happen to end. */
  float value_width = 0;
  for(unsigned int i = 0; i < items.size(); ++i)
  {
    float w = items[i]->get_value_width();
    if(w > value_width)
      value_width = w;
  }

  return value_width;
}

float
Menu::get_height() const
{
  return items.size() * 24;
}

void
Menu::hover_at(const Vector& mouse_pos)
{
  const float x = mouse_pos.x;
  const float y = mouse_pos.y;

  if(x > pos.x - get_width()/2 &&
     x < pos.x + get_width()/2 &&
     y > pos.y - get_height()/2 &&
     y < pos.y + get_height()/2)
  {
    int new_active_item
      = static_cast<int> ((y - (pos.y - get_height()/2)) / 24);

    /* only change the mouse focus to a selectable item */
    if (!items[new_active_item]->skippable())
      active_item = new_active_item;

    if(MouseCursor::current())
      MouseCursor::current()->set_state(MC_LINK);
  }
  else
  {
    if(MouseCursor::current())
      MouseCursor::current()->set_state(MC_NORMAL);
  }
}

void
Menu::on_window_resize()
{
  place_on_screen();

  /* The pointer has not moved, so no event will arrive to say which row it is
     over now, but the rows have moved under it. Work it out again from where
     the pointer actually is. */
  int x, y;
  SDL_GetMouseState(&x, &y);
  hover_at(VideoSystem::current()->get_renderer().to_logical(x, y));
}

void
Menu::place_on_screen()
{
  pos.x = SCREEN_WIDTH / 2;

  if (!placed_with_help())
  {
    pos.y = SCREEN_HEIGHT / 2;
    return;
  }

  /* The menu and the help box under it are one block, and the block goes in
     the middle of what the notice along the foot leaves free. The height comes
     from the tallest help in the menu rather than the row under the cursor, so
     that running down the items does not shift the menu about. */
  float tallest = 0.0f;
  for (const auto& item : items)
  {
    if (!item->help.empty())
    {
      tallest = std::max(tallest, Resources::normal_font->get_text_height(item->help));
    }
  }

  const float block = get_height() + MENU_PANEL_EDGE * 2 + HELP_GAP
                      + tallest + HELP_BOX_PADDING;

  /* Sat as low as the notice allows rather than centred in the room above it.
     The bottom only has to keep off the notice, while everything the block
     rises into at the top is picture, so the slack is better spent upwards. */
  const float block_top = SCREEN_HEIGHT - NOTICE_FOOT - NOTICE_GAP - block;

  pos.y = block_top + MENU_PANEL_EDGE + get_height()/2;
}

void
Menu::draw(DrawingContext& context)
{
  if (!items[active_item]->help.empty())
  {
    int text_width  = (int) Resources::normal_font->get_text_width(items[active_item]->help);
    int text_height = (int) Resources::normal_font->get_text_height(items[active_item]->help);

    /* The box hangs below the menu it belongs to, rather than sitting at a
       place on the screen chosen without reference to it. A menu is as tall as
       the items it holds, so anywhere fixed is over the foot of some menu and
       short of another. */
    const float top = pos.y + get_height()/2 + MENU_PANEL_EDGE + HELP_GAP;

    const float inner_top = top + HELP_BORDER;

    Rectf text_rect(pos.x - text_width/2 - HELP_PAD_X,
                   inner_top,
                   pos.x + text_width/2 + HELP_PAD_X,
                   inner_top + text_height + HELP_PAD_Y * 2);

    context.draw_filled_rect(Rectf(text_rect.p1 - Vector(HELP_BORDER, HELP_BORDER),
                                   text_rect.p2 + Vector(HELP_BORDER, HELP_BORDER)),
                             Color(0.2f, 0.3f, 0.4f, 0.8f),
                             16.0f,
                             LAYER_GUI-10);

    context.draw_filled_rect(text_rect,
                             Color(0.6f, 0.7f, 0.8f, 0.5f),
                             16.0f,
                             LAYER_GUI-10);

    context.draw_text(Resources::normal_font, items[active_item]->help,
                      Vector(pos.x, inner_top + HELP_PAD_Y),
                      ALIGN_CENTER, LAYER_GUI);
  }

  for(unsigned int i = 0; i < items.size(); ++i)
  {
    draw_item(context, i);
  }
}

MenuItem&
Menu::get_item_by_id(int id)
{
  for (const auto& item : items)
  {
    if (item->id == id)
    {
      return *item;
    }
  }

  throw std::runtime_error("MenuItem not found: " + std::to_string(id));
}

const MenuItem&
Menu::get_item_by_id(int id) const
{
  for (const auto& item : items)
  {
    if (item->id == id)
    {
      return *item;
    }
  }

  throw std::runtime_error("MenuItem not found");
}

int Menu::get_active_item_id() const
{
  return items[active_item]->id;
}

void
Menu::event(const SDL_Event& ev)
{
  items[active_item]->event(ev);
  switch(ev.type) {
    case SDL_MOUSEBUTTONDOWN:
    if(ev.button.button == SDL_BUTTON_LEFT)
    {
      Vector mouse_pos = VideoSystem::current()->get_renderer().to_logical(ev.motion.x, ev.motion.y);
      int x = int(mouse_pos.x);
      int y = int(mouse_pos.y);

      const float menu_width = get_width();
      if(x > pos.x - menu_width/2 &&
         x < pos.x + menu_width/2 &&
         y > pos.y - get_height()/2 &&
         y < pos.y + get_height()/2)
      {
        /* Ask the item what was pressed, since one drawn with an arrow at
           either end means different things at different places along it. */
        const MenuAction action = items[active_item]->get_click_action(
          x - (pos.x - menu_width/2), static_cast<int>(menu_width),
          get_value_width());

        if(action != MENU_ACTION_NONE)
        {
          process_action(action);
        }
      }
    }
    break;

    case SDL_MOUSEMOTION:
      hover_at(VideoSystem::current()->get_renderer().to_logical(ev.motion.x, ev.motion.y));
    break;

    default:
      break;
  }
}

void
Menu::set_active_item(int id)
{
  for(size_t i = 0; i < items.size(); ++i) {
    if(items[i]->id == id) {
      active_item = i;
      break;
    }
  }
}

bool
Menu::is_sensitive() const {
  return false;
}

/* EOF */
