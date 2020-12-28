/* netToe Version 1.5.1
 *
 * Copyright 2000,2001 Gabriele Giorgetti <ggdev@users.sourceforge.net>
 *           2011-2014 Mats Erik Andersson <meand@users.sourceforge.net>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _TERMINAL_H
# define _TERMINAL_H 1

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef COLOR_BLACK
enum {
	COLOR_BLACK, COLOR_RED, COLOR_GREEN,
	COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA,
	COLOR_CYAN, COLOR_WHITE, COLOR_DEFAULT = 9
};
#endif /* ECMA-48 colour definitions. */

#ifndef COLOR_DEFAULT
#  define COLOR_DEFAULT 9
#endif

enum {
	ATTRIB_RESET = 0, ATTRIB_BRIGHT, ATTRIB_DIM, ATTRIB_UNDERLINE = 4,
	ATTRIB_BLINK, ATTRIB_REVERSE = 7, ATTRIB_HIDDEN
};

#if defined USE_TERMINFO || defined USE_TERMCAP
void setup_terminfo(void);
#endif

void nettoe_term_clear (void);
void nettoe_term_set_color (int fg, int attrib);
void nettoe_term_set_default_color (void);
void nettoe_term_reset_color (void);

void nettoe_beep(void);

#endif /* !_TERMINAL_H */
