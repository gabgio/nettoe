/* netToe Version 1.5.1
 *
 * Copyright 2013, 2014 Mats Erik Andersson <meand@users.sourceforge.net>
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

#ifndef _BOARD_H
# define _BOARD_H

#define GAME_IS_ALIVE	0
#define GAME_IS_X_WIN	1
#define GAME_IS_O_WIN	2
#define GAME_IS_DRAW	3

#define INVALID_MOVE	0
#define VALID_MOVE	1

struct endpoints {
  int first, last;
};

# ifndef NO_EXTERNALS
extern char c11, c12, c13;
extern char c21, c22, c23;
extern char c31, c32, c33;

#  ifdef BOARD_CALCULATIONS
extern char *C11, *C12, *C13;
extern char *C21, *C22, *C23;
extern char *C31, *C32, *C33;

extern char * board[9];

extern struct endpoints winning_line[8];
#  endif /* BOARD_CALCULATIONS */
# endif /* !NO_EXTERNALS */

void init_matrix (void);
int game_check (void);
int attempt_move (const char *, char);
#endif /* _BOARD_H */
