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

#ifndef _MISC_H
# define _MISC_H 1

void parse_cmd_args(int argc, char *argv[]);

void get_player_pname(char * str, size_t maxlen);
int check_pname(const char *pname, size_t maxlen);

int who_start_first(void);
void print_infos_screen(void);

#endif /* !_MISC_H */
