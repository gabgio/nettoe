/* netToe Version 1.5.1
 *
 * Copyright  2011, 2012, 2013, 2014 Mats Erik Andersson
 *            <meand@users.sourceforge.net>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#if defined USE_TERMINFO || defined USE_TERMCAP
# if (defined HAVE_NCURSES_H || defined HAVE_NCURSES_NCURSES_H) \
	&& (defined USE_TINFO || defined USE_NCURSES)
#  if defined HAVE_NCURSES_NCURSES_H
#   include <ncurses/ncurses.h>
#  else
#   include <ncurses.h>
#  endif
# endif
# if defined HAVE_CURSES_H && defined USE_CURSES
#  include <curses.h>
# endif
# if defined HAVE_NCURSES_TERM_H && USE_NCURSES
#  include <ncurses/term.h>
# elif defined HAVE_TERM_H && !USE_TERMCAP
#  include <term.h>
# elif defined HAVE_TERMCAP_H && USE_TERMCAP
#  include <termcap.h>
# endif

# if !HAVE_PUTP
#  define putp(x)	tputs((x), 1, putchar)
# endif

const char *clear_str = NULL;
const char *bell_str = NULL;
const char *bold_str = NULL;
const char *rmso_str = NULL;
const char *smso_str = NULL;
const char *sgr0_str = NULL;
const char *setab_str = NULL;
const char *setaf_str = NULL;
#endif /* Use terminal capabilities */

int main(void)
{
#if defined USE_TERMINFO || defined USE_TERMCAP
   char *sp;
# if HAVE_SETUPTERM || HAVE_TGETENT
   int err;
# endif /* HAVE_SETUPTERM || HAVE_TGETENT */
# if HAVE_TGETENT
   static char buf[1024];
   char *p = buf;
# endif /* HAVE_TGETENT */

   if ((sp = getenv("TERM")) == NULL) {
      sp = "dumb";
      printf("No environment value set for TERM!\n"
	     "Using \"%s\" instead.\n", sp);
   }

# if HAVE_SETUPTERM
   if (setupterm(sp, fileno(stdout), &err) == ERR) {
      switch (err) {
	case 1: puts("Hardcopy terminal!");
		return EXIT_SUCCESS;
		break;
	case 0: printf("Terminal \"%s\" not found.\n", sp);
		return EXIT_SUCCESS;
		break;
	default: puts("No terminfo database accessible.");
		 return EXIT_SUCCESS;
		 break;
      }
   }
# elif HAVE_TGETENT
   err = tgetent(buf, sp);
   switch (err) {
     case 1: /* Success */
	     break;
     case 0: printf("Terminal \"%s\" not found.\n", sp);
	     return EXIT_SUCCESS;
	     break;
     default: puts("No terminfo database accessible.");
	      return EXIT_SUCCESS;
	      break;
   }
# endif /* !HAVE_SETUPTERM && HAVE_TGETENT */

   printf("Terminal: TERM = %s.\n", sp);

# if HAVE_SETUPTERM
   clear_str = tigetstr("clear");
   bell_str = tigetstr("bel");
   bold_str = tigetstr("bold");
   rmso_str = tigetstr("rmso");
   smso_str = tigetstr("smso");
   sgr0_str = tigetstr("sgr0");
   setab_str = tigetstr("setab");
   setaf_str = tigetstr("setaf");
# elif HAVE_TGETENT
   clear_str = tgetstr("cl", &p);
   bell_str  = tgetstr("bl", &p);
   bold_str  = tgetstr("md", &p);
   rmso_str  = tgetstr("se", &p);
   smso_str  = tgetstr("so", &p);
   sgr0_str  = tgetstr("me", &p);
   setab_str = tgetstr("AB", &p);
   setaf_str = tgetstr("AF", &p);
# endif /* HAVE_TGETENT */
   printf("clear: %s\n", clear_str ? "yes" : "no, using ANSI");
   printf("bell:  %s\n", bell_str ? "yes" : "no, using ANSI");
   printf("bold:  %s\n", bold_str ? "yes" : "no, using ANSI");
   printf("rmso:  %s\n", rmso_str ? "yes" : "no, using ANSI");
   printf("smso:  %s\n", smso_str ? "yes" : "no, using ANSI");
   printf("sgr0:  %s\n", sgr0_str ? "yes" : "no, using ANSI");
   printf("setab: %s\n", setab_str ? "yes" : "no, using ANSI");
   printf("setaf: %s\n", setaf_str ? "yes" : "no, using ANSI");

   return 0;
#else /* No terminal capabilities. */
   printf("No terminal capabilities are in use.\n");

   return 0;
#endif
}

