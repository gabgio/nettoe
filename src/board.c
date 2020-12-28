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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>		/* strncasecmp */
#endif

#define NO_EXTERNALS
#include "board.h"

char c11, c12, c13;
char c21, c22, c23;
char c31, c32, c33;

char *C11, *C12, *C13;
char *C21, *C22, *C23;
char *C31, *C32, *C33;

/* Board vector for unified calculations. */
char * board[9];

/* The eight possible winning lines expressed as indices.
 *
 * These give the first and last positions, the middle
 * position is directly retreived as the mean of those.
 *
 * A graphical translation is easy:
 *
 *   0 | 1 | 2
 *   ---------
 *   3 | 4 | 5   <====>   board[ 0 1 2 3 4 5 6 7 8 ]
 *   ---------
 *   6 | 7 | 8
 *
 * */
struct endpoints winning_line[8] =
  {
    { 0, 2 }, { 3, 5 }, { 6, 8 },	/* horizontals */
    { 0, 6 }, { 1, 7 }, { 2, 8 },	/* verticals */
    { 0, 8 }, { 2, 6 }			/* diagonals */
  };

static void switch_char_pointers(char ** a, char ** b) {
  char * tmp = *a;

  *a = *b;
  *b = tmp;
}

/* Transform board position to array index.
 * Here 1 <= row,col <= 3.
 */
static int pos_to_ind(char row, char col)
{
  return 3 * (row - 1) + (col - 1);
}

void init_matrix (void)
{
  c11 = ' ', c12 = ' ', c13 = ' ';
  c21 = ' ', c22 = ' ', c23 = ' ';
  c31 = ' ', c32 = ' ', c33 = ' ';
  C11 = &c11, C12 = &c12, C13 = &c13;
  C21 = &c21, C22 = &c22, C23 = &c23;
  C31 = &c31, C32 = &c32, C33 = &c33;

  /* Randomize internal board view.
   * An initial srandom(time(0)) was applied
   * in misc.c.
   */
  if ( random() % 2 ) {
	  /* Vertical mirroring. */
	switch_char_pointers(&C11, &C31);
	switch_char_pointers(&C12, &C32);
	switch_char_pointers(&C13, &C33);
  }
  if ( random() % 2 ) {
	  /* Horizontal mirroring. */
	switch_char_pointers(&C11, &C13);
	switch_char_pointers(&C21, &C23);
	switch_char_pointers(&C31, &C33);
  }
  if ( random() % 2 ) {
	  /* Diagonal mirroring. */
	switch_char_pointers(&C11, &C33);
	switch_char_pointers(&C12, &C23);
	switch_char_pointers(&C21, &C32);
  }

  /* Set up the board array. Used in AI-mode 3. */
  board[pos_to_ind(1,1)] = C11;
  board[pos_to_ind(1,2)] = C12;
  board[pos_to_ind(1,3)] = C13;
  board[pos_to_ind(2,1)] = C21;
  board[pos_to_ind(2,2)] = C22;
  board[pos_to_ind(2,3)] = C23;
  board[pos_to_ind(3,1)] = C31;
  board[pos_to_ind(3,2)] = C32;
  board[pos_to_ind(3,3)] = C33;
}

int game_check (void)
{
  int n;

  for (n = 0; n < 8; n++)
    {
      struct endpoints line = winning_line[n];

      if ( *board[line.first] == *board[line.last]
	  && *board[line.first] == *board[(line.first + line.last) / 2] )
	{
	  /* Disregard empty lines. */
	  if (*board[line.first] != 'X' && *board[line.first] != 'O')
	    continue;

	  /* Identical pawns in a winning line! */
	  return ((*board[line.first] == 'X') ? GAME_IS_X_WIN
					      : GAME_IS_O_WIN);
	}
    }

  /* Is the board exhausted? */
  if ( (c11 != ' ') && (c12 != ' ') && (c13 != ' ')
      && (c21 != ' ') && (c22 != ' ') && (c23 != ' ')
      && (c31 != ' ') && (c32 != ' ') & (c33 != ' ') )
    {
      return GAME_IS_DRAW;
    }

  return GAME_IS_ALIVE;
}

struct good_move {
  char * string;
  char * position;
} good_moves[] = {
  { "a1", &c11 },
  { "a2", &c12 },
  { "a3", &c13 },
  { "b1", &c21 },
  { "b2", &c22 },
  { "b3", &c23 },
  { "c1", &c31 },
  { "c2", &c32 },
  { "c3", &c33 },
#ifndef HAVE_STRNCASECMP
  { "A1", &c11 },
  { "A2", &c12 },
  { "A3", &c13 },
  { "B1", &c21 },
  { "B2", &c22 },
  { "B3", &c23 },
  { "C1", &c31 },
  { "C2", &c32 },
  { "C3", &c33 },
#endif /* !HAVE_STRNCASECMP */
  { NULL, NULL },
};

int attempt_move (const char *move, char pawn)
{
  struct good_move *p;

  for (p = good_moves; p->string && p->position; p++)
    {
#ifdef HAVE_STRNCASECMP
      if (!strncasecmp (move, p->string, strlen (p->string)))
#else /* !HAVE_STRNCASECMP */
      if (!strncmp (move, p->string, strlen (p->string)))
#endif
	{
	  if (*p->position == ' ')
	    {
	      *p->position = pawn;
	      return VALID_MOVE;
	    }
	}
    }

  return INVALID_MOVE;
}
