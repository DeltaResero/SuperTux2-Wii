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

#ifndef HEADER_SUPERTUX_GUI_MENU_HPP
#define HEADER_SUPERTUX_GUI_MENU_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <SDL.h>

#include "gui/menu_action.hpp"
#include "math/vector.hpp"

class Controller;
class DrawingContext;
class ItemAction;
class ItemBack;
class ItemControlField;
class ItemGoTo;
class ItemHorizontalLine;
class ItemInactive;
class ItemLabel;
class ItemStringSelect;
class ItemToggle;
class MenuItem;

class Menu
{
public:
  Menu();
  virtual ~Menu();

  virtual void menu_action(MenuItem& item) = 0;

  /** Executed before the menu is exited
      @return true if it should perform the back action, false if it shouldn't */
  virtual bool on_back_action() { return true; }

  /** Perform actions to bring the menu up to date with configuration changes */
  virtual void refresh() {}

  virtual void on_window_resize();

  ItemHorizontalLine& add_hl();
  ItemLabel& add_label(const std::string& text);
  ItemAction& add_entry(int id, const std::string& text);
  ItemAction& add_entry(const std::string& text, const std::function<void()>& callback);
  ItemToggle& add_toggle(int id, const std::string& text, bool* toggled);
  ItemToggle& add_toggle(int id, const std::string& text,
                         const std::function<bool()>& get_func,
                         const std::function<void(bool)>& set_func);
  ItemInactive& add_inactive(const std::string& text);
  ItemBack& add_back(const std::string& text, int id = -1);
  ItemGoTo& add_submenu(const std::string& text, int submenu, int id = -1);
  ItemControlField& add_controlfield(int id, const std::string& text, const std::string& mapping = "");
  ItemStringSelect& add_string_select(int id, const std::string& text, int* selected, const std::vector<std::string>& strings);

  void process_input(const Controller& controller);

  /** Remove all entries from the menu */
  void clear();

  MenuItem& get_item_by_id(int id);

  void draw(DrawingContext& context);
  Vector get_center_pos() const { return m_pos; }
  void set_center_pos(float x, float y);

  void event(const SDL_Event& event);

  float get_width() const;
  float get_height() const;

protected:
  MenuItem& add_item(std::unique_ptr<MenuItem> menu_item);

private:
  void process_action(const MenuAction& menuaction);
  void check_controlfield_change_event(const SDL_Event& event);
  void draw_item(DrawingContext& context, int index);

  /** The room the help box takes under the menu, or none when no item has help */
  float get_help_space() const;

  /** How many rows the window holds, and whether the menu has more than that */
  int get_window_rows() const;
  bool is_scrolling() const;

  /** Whether rows are scrolled out of the window above or below it */
  bool has_more_above() const;
  bool has_more_below() const;

  /** Which arrow is at a height: -1 for up, 1 for down, 0 for neither */
  int get_arrow_at(float y) const;

  /** Moves the rows shown by that many, as far as there are rows to show */
  void scroll(int rows);

  /** Puts the menu on screen and, after a key moves the selection, scrolls it into view */
  void place();

  /** Selects the row under the pointer, or notes the arrow it's on */
  void point_at(const Vector& mouse_pos);
  /** Recalculates the width for this menu */
  void calculate_width();

private:
  /** position of the menu (ie. center of the menu, not top/left) */
  Vector m_pos;

  /* input implementation variables */
  int m_delete_character;
  char m_mn_input_char;
  float m_menu_repeat_time;
  float m_menu_width;

public:
  std::vector<std::unique_ptr<MenuItem> > m_items;

private:
  int m_arrange_left;

protected:
  int m_active_item;

private:
  // The row under the pointer, or -1 when it's off the menu, and how far along that row it is
  int m_pointer_item;
  float m_pointer_x;

  // The first row shown when the menu is taller than its window, and whether a key just moved the selection
  int m_first_row;
  bool m_show_active;

  // The arrow under the pointer and the one held down, -1 for up and 1 for down, and when a held one scrolls again
  int m_pointer_arrow;
  int m_held_arrow;
  float m_arrow_repeat_time;

  // How far each arrow has eased into the highlight colour, and when they were last drawn
  float m_up_hover;
  float m_down_hover;
  float m_last_draw_time;

  // Where the pointer last was, so the row under it can be found again after the wheel scrolls
  Vector m_pointer_pos;

private:
  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;
};

#endif

/* EOF */
