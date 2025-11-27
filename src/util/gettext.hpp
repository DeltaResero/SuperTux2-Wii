// src/util/gettext.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_GETTEXT_HPP
#define HEADER_SUPERTUX_UTIL_GETTEXT_HPP

#include <tinygettext/tinygettext.hpp>
#include <assert.h>
#include <memory>

#include "supertux/globals.hpp"

extern std::unique_ptr<tinygettext::DictionaryManager> g_dictionary_manager;

/*
 * If you need to do a nontrivial substitution of values into a pattern, use
 * boost::format rather than an ad-hoc concatenation.  That way, translators can
 * translate the format string as a whole (and even rearrange the values if
 * necessary with "%1$s"-style codes) instead of multiple pieces.  Patterns like
 * "Label: %s" with only one string piece are a borderline case where
 * boost::format is not really necessary.
 *
 * http://www.mihai-nita.net/article.php?artID=20060430a
 *
 * Bad:
 *     std::string msg = _("You collected ") + num + _(" coins");
 *     std::cout << _("You collected ") << num << _(" coins");
 * Good:
 *     #include <boost/format.hpp>
 *     std::string msg = str(boost::format(_("You collected %d coins")) % num);
 *     std::cout << boost::format(_("You collected %d coins")) % num;
 */

static inline std::string _(const std::string& message)
{
  if (g_dictionary_manager)
  {
    return g_dictionary_manager->get_dictionary().translate(message);
  }
  else
  {
    return message;
  }
}

#endif // HEADER_SUPERTUX_UTIL_GETTEXT_HPP

// EOF
