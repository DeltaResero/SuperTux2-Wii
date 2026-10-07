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

#include <algorithm>
#include <cmath>

#include "control/input_manager.hpp"
#include "gui/item_action.hpp"
#include "gui/item_back.hpp"
#include "gui/item_controlfield.hpp"
#include "gui/item_goto.hpp"
#include "gui/item_hl.hpp"
#include "gui/item_inactive.hpp"
#include "gui/item_label.hpp"
#include "gui/item_stringselect.hpp"
#include "gui/item_toggle.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "gui/mousecursor.hpp"
#include "math/util.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

static const float MENU_REPEAT_INITIAL = 0.4f;
static const float MENU_REPEAT_RATE    = 0.1f;

namespace {

// How much of each edge a TV crops, as a share of the screen's height
const float SAFE_EDGE = 0.05f;

// Where the title screen's ice border begins, as a share of the screen's height
const float ICE_LINE = 5.0f / 6.0f;

// How far the panel MenuManager draws reaches past the rows
const float PANEL_EDGE = 14.0f;

// The space between the panel and the help box under it
const float HELP_GAP = 12.0f;

// Eases from the menu's own colour to its highlight colour as hover goes from 0 to 1
void draw_arrow(DrawingContext& context, const SurfacePtr& arrow, const Vector& pos, float angle, float hover)
{
  const Color& tint = ColorScheme::Menu::active_color;
  const Color color(1.0f + (tint.red - 1.0f) * hover,
                    1.0f + (tint.green - 1.0f) * hover,
                    1.0f + (tint.blue - 1.0f) * hover);
  context.color().draw_surface(arrow, pos, angle, color, Blend(), LAYER_GUI);
}

} // namespace

Menu::Menu() :
  m_pos(Vector(static_cast<float>(UI_WIDTH) / 2.0f,
               static_cast<float>(UI_HEIGHT) / 2.0f)),
  m_delete_character(0),
  m_mn_input_char('\0'),
  m_menu_repeat_time(),
  m_menu_width(),
  m_items(),
  m_arrange_left(0),
  m_active_item(-1),
  m_pointer_item(-1),
  m_pointer_x(0.0f),
  m_first_row(0),
  m_show_active(true),
  m_pointer_arrow(0),
  m_held_arrow(0),
  m_arrow_repeat_time(0.0f),
  m_up_hover(0.0f),
  m_down_hover(0.0f),
  m_last_draw_time(0.0f),
  m_pointer_pos(0.0f, 0.0f)
{
}

Menu::~Menu()
{
}

void
Menu::set_center_pos(float x, float y)
{
  m_pos.x = x;
  m_pos.y = y;
}

/* Add an item to a menu */
MenuItem&
Menu::add_item(std::unique_ptr<MenuItem> new_item)
{
  m_items.push_back(std::move(new_item));
  MenuItem& item = *m_items.back();

  /* If a new menu is being built, the active item shouldn't be set to
   * something that isn't selectable. Set the active_item to the first
   * selectable item added.
   */

  if (m_active_item == -1 && !item.skippable())
  {
    m_active_item = static_cast<int>(m_items.size()) - 1;
  }

  calculate_width();

  return item;
}

ItemHorizontalLine&
Menu::add_hl()
{
  auto item = std::make_unique<ItemHorizontalLine>();
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemLabel&
Menu::add_label(const std::string& text)
{
  auto item = std::make_unique<ItemLabel>(text);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemControlField&
Menu::add_controlfield(int id, const std::string& text,
                       const std::string& mapping)
{
  auto item = std::make_unique<ItemControlField>(text, mapping, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemAction&
Menu::add_entry(int id, const std::string& text)
{
  auto item = std::make_unique<ItemAction>(text, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemAction&
Menu::add_entry(const std::string& text, const std::function<void()>& callback)
{
  auto item = std::make_unique<ItemAction>(text, -1, callback);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemInactive&
Menu::add_inactive(const std::string& text)
{
  auto item = std::make_unique<ItemInactive>(text);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemToggle&
Menu::add_toggle(int id, const std::string& text, bool* toggled)
{
  auto item = std::make_unique<ItemToggle>(text, toggled, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemToggle&
Menu::add_toggle(int id, const std::string& text,
                 const std::function<bool()>& get_func,
                 const std::function<void(bool)>& set_func)
{
  auto item = std::make_unique<ItemToggle>(text, get_func, set_func, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemStringSelect&
Menu::add_string_select(int id, const std::string& text, int* selected, const std::vector<std::string>& strings)
{
  auto item = std::make_unique<ItemStringSelect>(text, strings, selected, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemBack&
Menu::add_back(const std::string& text, int id)
{
  auto item = std::make_unique<ItemBack>(text, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

ItemGoTo&
Menu::add_submenu(const std::string& text, int submenu, int id)
{
  auto item = std::make_unique<ItemGoTo>(text, submenu, id);
  auto item_ptr = item.get();
  add_item(std::move(item));
  return *item_ptr;
}

void
Menu::clear()
{
  m_items.clear();
  m_active_item = -1;
}

void
Menu::process_input(const Controller& controller)
{
  place();

  // A held arrow keeps scrolling while the pointer stays on it
  if (m_held_arrow != 0 && m_held_arrow == m_pointer_arrow && g_real_time > m_arrow_repeat_time)
  {
    scroll(m_held_arrow);
    m_arrow_repeat_time = g_real_time + MENU_REPEAT_RATE;
  }

  MenuAction menuaction = MenuAction::NONE;

  /** check main input controller... */
  if (controller.pressed(Control::UP)) {
    menuaction = MenuAction::UP;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_INITIAL;
  }
  if (controller.hold(Control::UP) &&
     m_menu_repeat_time != 0 && g_real_time > m_menu_repeat_time) {
    menuaction = MenuAction::UP;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_RATE;
  }

  if (controller.pressed(Control::DOWN)) {
    menuaction = MenuAction::DOWN;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_INITIAL;
  }
  if (controller.hold(Control::DOWN) &&
     m_menu_repeat_time != 0 && g_real_time > m_menu_repeat_time) {
    menuaction = MenuAction::DOWN;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_RATE;
  }

  if (controller.pressed(Control::LEFT)) {
    menuaction = MenuAction::LEFT;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_INITIAL;
  }
  if (controller.hold(Control::LEFT) &&
     m_menu_repeat_time != 0 && g_real_time > m_menu_repeat_time) {
    menuaction = MenuAction::LEFT;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_RATE;
  }

  if (controller.pressed(Control::RIGHT)) {
    menuaction = MenuAction::RIGHT;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_INITIAL;
  }
  if (controller.hold(Control::RIGHT) &&
     m_menu_repeat_time != 0 && g_real_time > m_menu_repeat_time) {
    menuaction = MenuAction::RIGHT;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_RATE;
  }

  if (controller.pressed(Control::ACTION) ||
     controller.pressed(Control::JUMP) ||
     controller.pressed(Control::MENU_SELECT) ||
     controller.pressed(Control::MENU_SELECT_SPACE)) {
    menuaction = MenuAction::HIT;
  }

  if (controller.pressed(Control::ESCAPE) ||
     controller.pressed(Control::CHEAT_MENU) ||
     controller.pressed(Control::DEBUG_MENU) ||
     controller.pressed(Control::MENU_BACK)) {
    menuaction = MenuAction::BACK;
  }

  if (controller.pressed(Control::REMOVE)) {
    menuaction = MenuAction::REMOVE;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_INITIAL;
  }
  if (controller.hold(Control::REMOVE) &&
     m_menu_repeat_time != 0 && g_real_time > m_menu_repeat_time) {
    menuaction = MenuAction::REMOVE;
    m_menu_repeat_time = g_real_time + MENU_REPEAT_RATE;
  }

  if (m_items.size() == 0)
    return;

  // The menu_action() call can pop() the menu from the stack and thus
  // delete it, so it's important that no further member variables are
  // accessed after this call
  process_action(menuaction);
}

void
Menu::process_action(const MenuAction& menuaction)
{
  const int last_active_item = m_active_item;

  switch (menuaction) {
    case MenuAction::UP:
      m_show_active = true;
      do {
        if (m_active_item > 0)
          --m_active_item;
        else
          m_active_item = int(m_items.size())-1;
      } while (m_items[m_active_item]->skippable()
               && (m_active_item != last_active_item));
      break;

    case MenuAction::DOWN:
      m_show_active = true;
      do {
        if (m_active_item < int(m_items.size())-1 )
          ++m_active_item;
        else
          m_active_item = 0;
      } while (m_items[m_active_item]->skippable()
               && (m_active_item != last_active_item));
      break;

    case MenuAction::BACK:
      if (on_back_action()) {
        MenuManager::instance().pop_menu();
      }
      return;

    default:
      break;
  }

  if (last_active_item != m_active_item) {
    // Selection caused by Up or Down keyboard action
    if (last_active_item != -1)
      m_items[last_active_item]->process_action(MenuAction::UNSELECT);
    m_items[m_active_item]->process_action(MenuAction::SELECT);
  }

  bool last_action = m_items[m_active_item]->no_other_action();
  m_items[m_active_item]->process_action(menuaction);
  if (last_action)
    return;

  if (m_items[m_active_item]->changes_width())
    calculate_width();
  if (menuaction == MenuAction::HIT)
    menu_action(*m_items[m_active_item]);
}

void
Menu::draw_item(DrawingContext& context, int index)
{
  const float menu_height = get_height();
  const float menu_width = get_width();

  MenuItem* pitem = m_items[index].get();

  const float x_pos = m_pos.x - menu_width / 2.0f;
  const int row = index - m_first_row;
  const float y_pos = m_pos.y + 24.0f * static_cast<float>(row) - menu_height / 2.0f + 12.0f;

  pitem->set_pointer_x(index == m_pointer_item ? std::optional<float>(m_pointer_x) : std::nullopt);
  pitem->draw(context, Vector(x_pos, y_pos), static_cast<int>(menu_width), m_active_item == index);

  if (m_active_item == index)
  {
    float blink = (sinf(g_real_time * math::PI * 1.0f)/2.0f + 0.5f) * 0.5f + 0.25f;
    context.color().draw_filled_rect(Rectf(Vector(m_pos.x - menu_width/2 + 10 - 2, y_pos - 12 - 2),
                                           Vector(m_pos.x + menu_width/2 - 10 + 2, y_pos + 12 + 2)),
                                     Color(1.0f, 1.0f, 1.0f, blink),
                                     14.0f,
                                     LAYER_GUI-10);
    context.color().draw_filled_rect(Rectf(Vector(m_pos.x - menu_width/2 + 10, y_pos - 12),
                                           Vector(m_pos.x + menu_width/2 - 10, y_pos + 12)),
                                     Color(1.0f, 1.0f, 1.0f, 0.5f),
                                     12.0f,
                                     LAYER_GUI-10);
  }
}

void
Menu::calculate_width()
{
  // Every item with a value gets the same column, so no arrow moves as a value changes
  float value_width = 0.0f;
  for (const auto& item : m_items)
  {
    value_width = std::max(value_width, item->get_value_width());
  }
  for (const auto& item : m_items)
  {
    item->set_value_column(value_width);
  }

  /* The width of the menu has to be more than the width of the text
     with the most characters */
  float max_width = 0;
  for (unsigned int i = 0; i < m_items.size(); ++i)
  {
    float w = static_cast<float>(m_items[i]->get_width());
    if (w > max_width)
      max_width = w;
  }
  m_menu_width = max_width;
}

float
Menu::get_width() const
{
  return m_menu_width + 24;
}

float
Menu::get_height() const
{
  return static_cast<float>(get_window_rows() * 24);
}

float
Menu::get_help_space() const
{
  // The tallest help, so the menu stays put as the selection moves
  float tallest = 0.0f;
  for (const auto& item : m_items)
  {
    if (!item->get_help().empty())
    {
      tallest = std::max(tallest, Resources::normal_font->get_text_height(item->get_help()));
    }
  }
  return (tallest > 0.0f) ? HELP_GAP + tallest + 16.0f : 0.0f;
}

int
Menu::get_window_rows() const
{
  const float top = static_cast<float>(UI_HEIGHT) * SAFE_EDGE;
  const float bottom = static_cast<float>(UI_HEIGHT) * ICE_LINE - get_help_space();
  const int fit = std::max(3, static_cast<int>((bottom - top - PANEL_EDGE * 2.0f) / 24.0f));
  return std::min(static_cast<int>(m_items.size()), fit);
}

int
Menu::get_arrow_at(float y) const
{
  const int row = static_cast<int>((y - (m_pos.y - get_height() / 2.0f)) / 24.0f);
  if (row == 0 && has_more_above())
    return -1;
  if (row == get_window_rows() - 1 && has_more_below())
    return 1;
  return 0;
}

bool
Menu::has_more_above() const
{
  return m_first_row > 0;
}

bool
Menu::has_more_below() const
{
  return m_first_row + get_window_rows() < static_cast<int>(m_items.size());
}

bool
Menu::is_scrolling() const
{
  return get_window_rows() < static_cast<int>(m_items.size());
}

void
Menu::scroll(int rows)
{
  m_first_row = std::clamp(m_first_row + rows, 0, static_cast<int>(m_items.size()) - get_window_rows());
}

void
Menu::place()
{
  const int count = static_cast<int>(m_items.size());
  const float top = static_cast<float>(UI_HEIGHT) * SAFE_EDGE;
  const float bottom = static_cast<float>(UI_HEIGHT) * ICE_LINE;

  if (is_scrolling())
  {
    // Too tall for the screen, so the window fills the space between the TV's edge and the ice and its rows scroll
    m_pos.y = std::floor((top + bottom - get_help_space()) / 2.0f);

    if (m_show_active && m_active_item >= 0)
    {
      int first_idx = count;
      int last_idx = count;
      for (int i = 0; i < count; ++i)
      {
        if (!m_items[i]->skippable())
        {
          if (first_idx == count)
            first_idx = i;
          last_idx = i;
        }
      }

      // The first item shows the menu's title above it, the last shows everything below it, and none sits under an arrow
      const int rows = get_window_rows();
      if (m_active_item == first_idx)
        m_first_row = 0;
      else if (m_active_item == last_idx)
        m_first_row = count;
      else if (m_active_item < m_first_row + (has_more_above() ? 1 : 0))
        m_first_row = m_active_item - 1;
      else if (m_active_item > m_first_row + rows - 1 - (has_more_below() ? 1 : 0))
        m_first_row = m_active_item - rows + 2;
    }
    scroll(0);
  }
  else
  {
    // Where the menu asked to be, moved only as far as it takes to stay between the TV's edge and the ice
    m_first_row = 0;
    const float block_top = m_pos.y - get_height() / 2.0f - PANEL_EDGE;
    const float block_bottom = m_pos.y + get_height() / 2.0f + PANEL_EDGE + get_help_space();
    if (block_bottom > bottom)
    {
      m_pos.y = std::floor(m_pos.y - (block_bottom - bottom));
    }
    else if (block_top < top)
    {
      m_pos.y = std::ceil(m_pos.y + (top - block_top));
    }
  }
  m_show_active = false;
}

void
Menu::on_window_resize()
{
  m_pos.x = static_cast<float>(UI_WIDTH) / 2.0f;
  m_pos.y = static_cast<float>(UI_HEIGHT) / 2.0f;
}

void
Menu::draw(DrawingContext& context)
{
  // A resize can move the menu between two game steps, so it's placed again before every frame
  place();

  // The top and bottom rows show an arrow instead of their item while there's more that way
  scroll(0);
  for (int i = m_first_row + (has_more_above() ? 1 : 0);
       i < m_first_row + get_window_rows() - (has_more_below() ? 1 : 0); ++i)
  {
    draw_item(context, i);
  }

  if (is_scrolling())
  {
    // The arrow under the pointer eases into the highlight colour, as a value's arrows do
    const float dt = std::clamp(g_real_time - m_last_draw_time, 0.0f, 0.1f);
    m_last_draw_time = g_real_time;
    const float ease = std::min(1.0f, dt * 15.0f);
    m_up_hover += ((m_pointer_arrow < 0 ? 1.0f : 0.0f) - m_up_hover) * ease;
    m_down_hover += ((m_pointer_arrow > 0 ? 1.0f : 0.0f) - m_down_hover) * ease;

    // The side arrows turned to point up and down
    const float arrow_left = m_pos.x - static_cast<float>(Resources::arrow_left->get_width()) / 2.0f;
    const float arrow_half = static_cast<float>(Resources::arrow_left->get_height()) / 2.0f;
    if (has_more_above())
    {
      draw_arrow(context, Resources::arrow_left, Vector(arrow_left, m_pos.y - get_height() / 2.0f + 12.0f - arrow_half),
                 90.0f, m_up_hover);
    }
    if (has_more_below())
    {
      draw_arrow(context, Resources::arrow_right, Vector(arrow_left, m_pos.y + get_height() / 2.0f - 12.0f - arrow_half),
                 90.0f, m_down_hover);
    }
  }

  const float panel_bottom = m_pos.y + get_height() / 2.0f + PANEL_EDGE;

  if (!m_items[m_active_item]->get_help().empty())
  {
    const int text_width = static_cast<int>(Resources::normal_font->get_text_width(m_items[m_active_item]->get_help()));
    const int text_height = static_cast<int>(Resources::normal_font->get_text_height(m_items[m_active_item]->get_help()));

    // Hung under the menu it describes
    const float help_top = panel_bottom + HELP_GAP + 4.0f;
    const Rectf text_rect(m_pos.x - static_cast<float>(text_width) / 2.0f - 8.0f,
                          help_top,
                          m_pos.x + static_cast<float>(text_width) / 2.0f + 8.0f,
                          help_top + static_cast<float>(text_height) + 8.0f);

    context.color().draw_filled_rect(Rectf(text_rect.p1() - Vector(4,4),
                                           text_rect.p2() + Vector(4,4)),
                                     Color(0.5f, 0.6f, 0.7f, 0.8f),
                                     16.0f,
                                     LAYER_GUI);

    context.color().draw_filled_rect(text_rect,
                                     Color(0.8f, 0.9f, 1.0f, 0.5f),
                                     16.0f,
                                     LAYER_GUI);

    context.color().draw_text(Resources::normal_font, m_items[m_active_item]->get_help(),
                              Vector(m_pos.x, help_top + 4.0f),
                              ALIGN_CENTER, LAYER_GUI);
  }
}

MenuItem&
Menu::get_item_by_id(int id)
{
  auto item = std::find_if(m_items.begin(), m_items.end(), [id](const std::unique_ptr<MenuItem>& i)
  {
    return i->get_id() == id;
  });

  if(item != m_items.end())
    return *item->get();

  throw std::runtime_error("MenuItem not found: " + std::to_string(id));
}

void
Menu::point_at(const Vector& mouse_pos)
{
  m_pointer_pos = mouse_pos;
  const float x = mouse_pos.x;
  const float y = mouse_pos.y;

  if (x > m_pos.x - get_width()/2 &&
      x < m_pos.x + get_width()/2 &&
      y > m_pos.y - get_height()/2 &&
      y < m_pos.y + get_height()/2)
  {
    m_pointer_arrow = get_arrow_at(y);
    if (m_pointer_arrow != 0)
    {
      m_pointer_item = -1;
      if (MouseCursor::current())
        MouseCursor::current()->set_state(MouseCursorState::LINK);
      return;
    }

    const int row = static_cast<int>((y - (m_pos.y - get_height() / 2.0f)) / 24.0f);
    int new_active_item = std::clamp(m_first_row + row, 0, static_cast<int>(m_items.size()) - 1);

    m_pointer_item = new_active_item;
    m_pointer_x = x - (m_pos.x - get_width() / 2.0f);

    /* only change the mouse focus to a selectable item */
    if (!m_items[new_active_item]->skippable() &&
        new_active_item != m_active_item) {
      // Selection caused by mouse movement
      if (m_active_item != -1)
        process_action(MenuAction::UNSELECT);
      m_active_item = new_active_item;
      process_action(MenuAction::SELECT);
    }

    if (MouseCursor::current())
      MouseCursor::current()->set_state(MouseCursorState::LINK);
  }
  else
  {
    m_pointer_item = -1;
    m_pointer_arrow = 0;

    if (MouseCursor::current())
      MouseCursor::current()->set_state(MouseCursorState::NORMAL);
  }
}

void
Menu::event(const SDL_Event& ev)
{
  m_items[m_active_item]->event(ev);
  switch (ev.type)
  {
    case SDL_KEYDOWN:
    case SDL_TEXTINPUT:
      if (((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_BACKSPACE) ||
         ev.type == SDL_TEXTINPUT) && m_items[m_active_item]->changes_width())
      {
        // Changed item value? Let's recalculate width:
        calculate_width();
      }
    break;

    case SDL_MOUSEBUTTONDOWN:
    if (ev.button.button == SDL_BUTTON_LEFT)
    {
      Vector mouse_pos = VideoSystem::current()->get_viewport().to_ui(ev.motion.x, ev.motion.y);

      if (mouse_pos.x > m_pos.x - get_width() / 2.0f &&
          mouse_pos.x < m_pos.x + get_width() / 2.0f &&
          mouse_pos.y > m_pos.y - get_height() / 2.0f &&
          mouse_pos.y < m_pos.y + get_height() / 2.0f)
      {
        const int arrow = get_arrow_at(mouse_pos.y);
        if (arrow != 0)
        {
          // A click scrolls a row and holding it keeps scrolling
          scroll(arrow);
          m_held_arrow = arrow;
          m_arrow_repeat_time = g_real_time + MENU_REPEAT_INITIAL;
        }
        else if (m_active_item >= 0)
        {
          // The item knows what it drew where, such as a value's arrows
          const float x = mouse_pos.x - (m_pos.x - get_width() / 2.0f);
          process_action(m_items[m_active_item]->get_click_action(x, static_cast<int>(get_width())));
        }
        else
        {
          process_action(MenuAction::HIT);
        }
      }
    }
    break;

    case SDL_MOUSEBUTTONUP:
      if (ev.button.button == SDL_BUTTON_LEFT)
        m_held_arrow = 0;
      break;

    case SDL_MOUSEWHEEL:
      if (is_scrolling())
      {
        scroll((ev.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? ev.wheel.y : -ev.wheel.y);
        // The rows moved under the pointer, so it now points at another
        if (m_pointer_item >= 0)
          point_at(m_pointer_pos);
      }
      break;

    case SDL_MOUSEMOTION:
      point_at(VideoSystem::current()->get_viewport().to_ui(ev.motion.x, ev.motion.y));
      break;

    default:
      break;
  }
}

/* EOF */
