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
#include <unistd.h>

#define BOARD_CALCULATIONS
#include "board.h"

/* A vector of all &C11, &C12, etcetera, ordered
 * in some way to reflect a strategic decision.
 * Used by AI to locate an unused field.
 */
static char ** pawnlist_old[] =
    { &C22, &C31, &C12, &C33, &C23, &C21, &C13, &C11, &C32, NULL };

/* Description of threat used by AI to decide on moves. */
struct probes {
  char pawn;		/* Probe only this kind of marker. */
  char ** first;	/* Two taken fields. */
  char ** second;
  char ** open;		/* One field open for play. */
};

static struct probes probelist_weak[] = {
  { 'X', &C11, &C12, &C13 },	/* Protect first row.	*/
  { 'X', &C33, &C32, &C31 },	/* Protect third row.	*/
  { 'X', &C11, &C21, &C31 },	/* Protect first column. */
  { 'X', &C13, &C23, &C33 },	/* Protect third column. */
  { 'X', &C13, &C22, &C31 },	/* Protect second diagonal. */
  { 'X', &C21, &C22, &C23 },	/* Protect second row.	*/
  { 'X', &C23, &C22, &C21 },	/* Protect second row.	*/
  { 'X', &C11, &C13, &C12 },	/* Protect first row.	*/
  { 'O', &C31, &C22, &C13 },	/* Attack second diagonal, win at corner. */
  { 'O', &C33, &C22, &C11 },	/* Attack first diagonal, win at corner. */
  { 'O', &C32, &C22, &C12 },	/* Attack second column, win at edge. */
  { 'O', &C12, &C22, &C32 },	/* Attack second column, win at edge. */
  { 0, NULL, NULL, NULL },
};

static struct probes probelist_standard[] = {
  { 'O', &C11, &C22, &C33 },	/* Attack first diagonal, win at corner. */
  { 'O', &C31, &C33, &C32 },	/* Attack third row, win at edge.	*/
  { 'O', &C23, &C22, &C21 },	/* Attack second row, win at edge.	*/
  { 'O', &C12, &C22, &C32 },	/* Attack second column, win at edge.	*/
  { 'O', &C13, &C22, &C31 },	/* Attack second diagonal, win at corner. */
  { 'O', &C31, &C22, &C13 },	/* Attack second diagonal, win at corner. */
  { 'O', &C33, &C22, &C11 },	/* Attack first diagonal, win at corner. */
  { 'O', &C32, &C22, &C12 },	/* Attack second column, win at edge.	*/
  { 'X', &C11, &C12, &C13 },	/* Protect first row.		*/
  { 'X', &C33, &C32, &C31 },	/* Protect third row.		*/
  { 'X', &C11, &C21, &C31 },	/* Protect first column.	*/
  { 'X', &C13, &C23, &C33 },	/* Protect third column.	*/
  { 'X', &C13, &C22, &C31 },	/* Protect second diagonal.	*/
  { 'X', &C21, &C22, &C23 },	/* Protect second row.		*/
  { 'X', &C23, &C22, &C21 },	/* Protect second row.		*/
  { 'X', &C11, &C13, &C12 },	/* Protect first row.		*/
  { 'X', &C12, &C22, &C32 },	/* Protect second column.	*/
  { 'X', &C32, &C22, &C12 },	/* Protect second column.	*/
  { 0, NULL, NULL, NULL },
};

/* The argument is a fixed, ordered vector of all &C11, &C12, etcetera.
 * It represents a particular strategy for finding empty fields.
 * A NULL at the end is a guard, detected by claim_empty_field().
 */
static void claim_empty_field (char **plist[])
{
  char ***p;

  for (p = plist; *p; p++)
    if (***p == ' ')
      {
	***p = 'O';
	return;
      }
}

static int play_block_or_win (char marker, int first, int last)
{
  int n;
  int marker_count = 0, blank_count = 0;

  for ( n=first; n<=last; n += (last - first)/2 )
  {
    if ( *board[n] == ' ')
    {
      blank_count++;
    }
    else if ( *board[n] == marker )
    {
      marker_count++;
    }
  }

  if ( blank_count == 1 && marker_count == 2 )
  {
    int play_this_pos;

    /* Is winning line or a serious threat. */
    if ( *board[first] == ' ' )
    {
      play_this_pos = first;
    }
    else if ( *board[last] == ' ' )
    {
      play_this_pos = last;
    }
    else
    {
      play_this_pos = (first + last)/2;
    }

    *board[play_this_pos] = 'O';

    /* Report a performed play. */
    return 1;
  }

  /* Nothing done. */
  return 0;
}

static int probe_for_fields (struct probes probelist[])
{
  struct probes *p;

  for (p = probelist; p->pawn; p++)
    {
      struct probes probe = *p;

      if (   (**probe.first  == probe.pawn)
	  && (**probe.second == probe.pawn)
	  && (**probe.open   == ' ') )
	{
	  **probe.open = 'O';
	  return 1;
	}
    }

  /* No action. */
  return 0;
}

static void get_cpu_normal_move (void)
{
  sleep ( 1 );

  if (probe_for_fields (probelist_weak))
    return;

  /* Find any unoccupied position. */
  claim_empty_field (pawnlist_old);
}

static void get_cpu_hard_move (void)
{
  sleep ( 1 );

  if (probe_for_fields (probelist_standard))
    return;

  /* Find any unoccupied position. */
  claim_empty_field (pawnlist_old);
}

static void get_cpu_harder_move (void)
{
  int n;

  sleep ( 1 );

  /* Locate any winning move. */
  for (n = 0; n < 8; n++)
  {
    if ( play_block_or_win('O',
		winning_line[n].first, winning_line[n].last) )
      return;
  }

  /* Locate any threat. */
  for (n = 0; n < 8; n++)
  {
    if ( play_block_or_win('X',
		winning_line[n].first, winning_line[n].last) )
      return;
  }

  /* Find any unoccupied position. */
  claim_empty_field (pawnlist_old);
}

void get_cpu_move (int level)
{
  switch (level)
    {
      case 1:
	get_cpu_normal_move ();
	break;
      case 2:
	get_cpu_hard_move ();
	break;
      case 3:
      default:
	get_cpu_harder_move ();
	break;
    }
}

