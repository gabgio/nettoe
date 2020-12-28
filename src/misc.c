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
#include <unistd.h>
#include <string.h>
#include <time.h>

/* Macros AF_* for address families. */
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
#endif 
#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#include "game.h"
#include "misc.h"
#include "terminal.h"

int who_start_first(void)
{
	long generated_number;

	generated_number = 1 + (random() % 10);

	if ( (generated_number == 2) || (generated_number == 4)
			|| (generated_number == 6) || (generated_number == 8)
			|| (generated_number == 10) )
	{
		return 1;
	}
 
	return 2;
} /* who_start_first(void) */

void get_player_pname(char * str, size_t maxlen)
{
	char buffer[1024], *pek;
	size_t n = 0;

	str[0] ='\0';
	if ( maxlen <= 1 )
		return;

	if (!fgets(buffer, sizeof(buffer), stdin))
	  buffer[0] = '\0';

	pek = buffer;

	/* Skip initial white space. */
	while ( *pek == ' ' || *pek == '\t' || *pek == '\r' )
		++pek;

	while ( *pek && (n + 1 < maxlen) && (n + 1 < sizeof(buffer)) )
	{
		if ( *pek == '\n' )
			break;

		/* Promote LF and TAB to simple space. */
		if ( *pek == '\r' || *pek == '\t' )
			*pek = ' ';

		/* Avoid duplicate white space. */
		if ( (n > 0) && (str[n - 1] == ' ') && (*pek == ' ') )
		{
			++pek;
			continue;
		}

		str[n++] = *(pek++);
	}

	/* Skip trailing white space. */
	while ( (n > 0) && (str[n - 1] == ' ') )
		--n;

	/* Safe guard for empty name. */
	if ( n == 0 )
		strcpy(str, "Anon");
	else
		str[n] = '\0';
} /* get_player_pname(char *, size_t) */

int check_pname(const char *pname, size_t maxlen)
{
	if ( pname == NULL )
		return 1;
   
	if ( (strlen(pname)) > maxlen )
		return 1;
   
	return 0;
} /* check_pname(const char *, size_t) */

void print_infos_screen (void)
{
	nettoe_term_reset_color();
	printf(" netToe is a Tic Tac Toe-like game for Linux and UNIX.    \n");
	printf(" It is possible to play it against the computer, another  \n");
	printf(" player on the same PC, or against another player over    \n");
	printf(" a network (Internet, and everything using TCP/IP).       \n");
	printf(" To play it over a network you must first set up a server.\n");
	printf(" This is done in the network game options menu, selecting \n");
	printf(" \"Host the game\". Then the (remote) second player must  \n");
	printf(" connect to the server by typing its IP address, or name. \n");
	printf(" That should be enough for you to have some leisure.      \n");
	printf(" For a detailed guide on HOW TO PLAY, and for other info, \n");
	printf(" do read the manual page ");
	nettoe_term_set_color (COLOR_BLUE, ATTRIB_BRIGHT);
	printf("nettoe");
	nettoe_term_reset_color();
	printf("(6).\n\n");

	printf(" netToe %s (%s)\n", PACKAGE_VERSION, PACKAGE_RELEASE_DATE);
	printf(" Copyright 2000,2001 Gabriele Giorgetti\n");
	printf("           2009-2014 Mats Erik Andersson\n");
	printf(" %s\n", HOMEPAGE);
	printf("\n Press");
	nettoe_term_set_color (COLOR_RED, ATTRIB_BRIGHT);
	printf(" enter");
	nettoe_term_reset_color();
	printf(" to go back to the main menu. ");
	 
	fflush (stdin);
	getchar ();

	nettoe_term_set_default_color();
} /* print_infos_screen(void) */

void parse_cmd_args(int argc, char *argv[])
{
	int i;

	/* Any address family will do. */
	addrfamily = AF_UNSPEC;

	if ( argc < 2 )
		return;

	for (i = 1; i <= argc - 1; i++)
	{
		if ( (!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")) )
		{
			fprintf(stdout, "netToe %s the enhanced, and networked, "
					"Tic Tac Toe game.\n", PACKAGE_VERSION);
			fprintf(stdout, "\n");
			fprintf(stdout, "Usage:  nettoe [OPTIONS]\n");
			fprintf(stdout, "\n");
			fprintf(stdout, "-4,  --ipv4        only IPv4 networking\n");
			fprintf(stdout, "-6,  --ipv6        only IPv6 networking\n");
			fprintf(stdout, "-nb, --no-beep     disable beeps\n");
			fprintf(stdout, "-nc, --no-colors   disable colors\n");
			fprintf(stdout, "-h,  --help        display this help,"
						    " then exit\n");
			fprintf(stdout, "-v,  --version     output version information,"
							" then exit\n");
			fprintf(stdout, "\n");
			fprintf(stdout, "The netToe project can be found at: \n");
			fprintf(stdout, "  %s\n", HOMEPAGE);
			fprintf(stdout, "\n");
			fprintf(stdout, "Please send any bug reports, or comments to:\n");
			fprintf(stdout, "  %s\n", AUTHOR2_EMAIL);

			exit(EXIT_SUCCESS);
		}
		else if ( (!strcmp(argv[i], "-v"))
				|| (!strcmp(argv[i], "--version")) )
		{
			fprintf(stdout, "netToe %s (%s)",
				PACKAGE_VERSION, RELEASE_DATE);
#if USE_TERMINFO
			fprintf(stdout, " with terminfo support");
#elif USE_TERMCAP
			fprintf(stdout, " with termcap support");
#endif
			fprintf(stdout, ", at TCP port %d.", SERVER_PORT_NUMBER);
			fprintf(stdout, "\n\n");
			fprintf(stdout, "Written by Gabriele Giorgetti %s\n",
					AUTHOR_EMAIL);
			fprintf(stdout, "Copyright 2000,2001 Gabriele Giorgetti\n");
			fprintf(stdout, "          2009-2014 Mats Erik Andersson\n");
			fprintf(stdout, "\n");
			fprintf(stdout, "This software is released under GNU GPL 2.\n");
			
			exit(EXIT_SUCCESS);
		}
		else if ( (!strcmp(argv[i], "-4"))
				|| (!strcmp(argv[i], "--ipv4")) )
		{
			addrfamily = AF_INET;
			continue;
		}
		else if ( (!strcmp(argv[i], "-6"))
				|| (!strcmp(argv[i], "--ipv6")) )
		{
			addrfamily = AF_INET6;
			continue;
		}
		else if ( (!strcmp(argv[i], "-nb"))
				|| (!strcmp(argv[i], "--no-beep")) )
		{
			NO_BEEP = 1;
			continue;
		}
		else if ( (!strcmp(argv[i], "-nc"))
				|| (!strcmp(argv[i], "--no-colors")) )
		{
			NO_COLORS = 1;
			continue;
		} else {
			fprintf(stdout, "%s: unrecognized option `%s'\n",
					argv[0], argv[i]);
			fprintf(stdout, "Try `%s --help' for more information.\n",
					argv[0]);

			exit (EXIT_SUCCESS);
		}
	}

	return;
} /* parse_cmd_args(int, char *[]) */

/*
 * vim: sw=4 ts=4
 */
