/* netToe Version 1.5.1
 *
 * Copyright 2000,2001 Gabriele Giorgetti <ggdev@users.sourceforge.net>
 *           2009-2014 Mats Erik Andersson <meand@users.sourceforge.net>
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
# endif /* NCURSES */
# if defined HAVE_CURSES_H && defined USE_CURSES
#  include <curses.h>
# endif /* CURSES */
# if defined HAVE_NCURSES_TERM_H && USE_NCURSES
#  include <ncurses/term.h>
# elif defined HAVE_TERM_H && !USE_TERMCAP
#  include <term.h>
# elif defined HAVE_TERMCAP_H && USE_TERMCAP
#  include <termcap.h>
# endif /* TERM || TERMCAP */

# if !HAVE_PUTP
#  define putp(x)	tputs((x), 1, putchar)
# endif
#endif /* Terminal capabilities. */

#include "terminal.h"
#include "game.h"

#if defined USE_TERMINFO || defined USE_TERMCAP
static char *clear_str = NULL;
static char *bell_str = NULL;
static char *bold_str = NULL;
static char *rmso_str = NULL;
static char *smso_str = NULL;
static char *sgr0_str = NULL;
static char *setab_str = NULL;
static char *setaf_str = NULL;

static int using_terminfo = 0;

void setup_terminfo (void)
{
# if HAVE_SETUPTERM
   int err;

   /* Error can be ignored, since we fall back
    * to ECMA/ANSI character sequences. Catching
    * errors in ERR is needed to avoid error output. */
   if (setupterm(NULL, fileno(stdout), &err) == OK)
     using_terminfo = 1;
# elif HAVE_TGETENT
   static char buf[1024];
   char *p = buf, *sp;

   if ((sp = getenv("TERM")) == NULL)
     sp = "vt100"; /* Hoping for the best. */

   /* Error can be ignored, since we fall back
    * to ECMA/ANSI character sequences. */
   if (tgetent(buf, sp) == 1)
     using_terminfo = 1;
# endif /* !HAVE_SETUPTERM && HAVE_TGETENT */

# if USE_TERMINFO
   if (using_terminfo)
     {
	clear_str = tigetstr("clear");
	bell_str = tigetstr("bel");
	bold_str = tigetstr("bold");
	rmso_str = tigetstr("rmso");
	smso_str = tigetstr("smso");
	sgr0_str = tigetstr("sgr0");
	setab_str = tigetstr("setab");
	setaf_str = tigetstr("setaf");
     }
# elif USE_TERMCAP
   if (using_terminfo)
     {
	clear_str = tgetstr("cl", &p);
	bell_str  = tgetstr("bl", &p);
	bold_str  = tgetstr("md", &p);
	rmso_str  = tgetstr("se", &p);
	smso_str  = tgetstr("so", &p);
	sgr0_str  = tgetstr("me", &p);
	setab_str = tgetstr("AB", &p);
	setaf_str = tgetstr("AF", &p);
     }
# endif /* USE_TERMCAP */
}
#endif /* Terminal capabilities. */


void nettoe_term_clear (void)
{
#if USE_TERMINFO || USE_TERMCAP
   if (clear_str)
     putp(clear_str);
   else
#endif
   {
     if (system("clear 2>/dev/null"))
       fprintf(stderr, "Terminal access failed.\n");
   }
}


/* same as nettoe_term_reset_color ( void ) */
void nettoe_term_set_default_color (void)
{
   if (NO_COLORS == 1)
     return;

#if USE_TERMINFO || USE_TERMCAP
   if (setaf_str)
     {
        /* Based on ECMA-48. */
# if USE_TERMINFO
        putp(tparm(setaf_str, COLOR_DEFAULT));

        if (setab_str)
           putp(tparm(setab_str, COLOR_DEFAULT));
# else /* USE_TERMCAP */
	printf(setaf_str, COLOR_DEFAULT);

        if (setab_str)
           printf(setab_str, COLOR_DEFAULT);
# endif /* USE_TERMCAP */

        return;
     }
#endif /* USE_TERMINFO || USE_TERMCAP */
   /* nettoe_term_set_color (COLOR_WHITE, ATTRIB_RESET); */

   return;
}


void nettoe_term_reset_color ( void )
{
   if (NO_COLORS == 1)
     return;

#if USE_TERMINFO || USE_TERMCAP
   if (sgr0_str)
     putp(sgr0_str);
   else if (rmso_str)
     putp(rmso_str);
   else
#endif /* USE_TERMINFO || USE_TERMCAP */
     printf("%c[0m", 0x1B);

   return;
}


void nettoe_term_set_color (int fg, int attrib)
{
   char cmd_str[13];

   if (NO_COLORS == 1 )
        return;

   if ( fg != COLOR_WHITE)
     {
        /* cmd_str is the control command to the terminal */
        sprintf(cmd_str, "%c[%d;%dm", 0x1B, attrib, fg + 30);
     }
   else
     {
        /* Text colour COLOR_WHITE is left for the terminal to decide. */
        sprintf(cmd_str, "%c[%dm", 0x1B, attrib);
     }

#if USE_TERMINFO || USE_TERMCAP
   if (setaf_str && (fg != COLOR_WHITE))
     {
        if (bold_str)
          putp(bold_str);
# if USE_TERMINFO
        putp(tparm(setaf_str, fg));
# else /* USE_TERMCAP */
        printf(setaf_str, fg);
# endif /* USE_TERMCAP */
     }
   else
#endif /* USE_TERMINFO || USE_TERMCAP */
   printf("%s", cmd_str);

   return;
}

void nettoe_beep (void)
{
   if ( NO_BEEP == 1 )
     return;

#if USE_TERMINFO || USE_TERMCAP
   if (bell_str)
     putp(bell_str);
   else
#endif /* USE_TERMINFO || USE_TERMCAP */
     printf("\a");
} /* beep(void) */
