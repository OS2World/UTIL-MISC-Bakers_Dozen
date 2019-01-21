/* $title:       'LIFE.C ==== Classic game of life =====' */
/* $subtitle:   'Public domain as noted     1994 Dec 15' */

/*
** A quick "life" (2-d cellular automaton) implementation done in Turbo C 2.0
** on the spur-of-the-moment by Jonathan Guthrie 9/20/1992 and donated to the
** public domain.
**------------------------------------------------------------------------
** these are the orginal comments. I ported it to OS/2 using Watcom C++
** 9.5. I changed it to use the VIO stuff (since that's about all I know..)
** OS/2 port by Jay Collins of FIDO: 1:275/158
*/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#define INCL_VIO
#include <os2.h>

#ifndef random
 #define random(num) (int)(((long)rand()*(num))/RAND_MAX)
#endif

/*
**  From VIDPORT.C, also in SNIPPETS
*/

#ifndef randomize
 #define randomize() srand(((unsigned int)time(NULL))|1)
#endif

#define     ROWS        23
#define     COLS        80
#define     GENERATIONS 10

int civ1[ROWS][COLS], civ2[ROWS][COLS];

void update_generation(int old[ROWS][COLS], int new[ROWS][COLS])
{
      int i, j, count;

      for (i = 0; i < ROWS; ++i)
      {
            for (j = 0; j < COLS; ++j)
            {
                  count = old[(i + ROWS - 1) % ROWS][(j + COLS - 1) % COLS] +
                        old[(i + ROWS - 1) % ROWS][j] +
                        old[(i + ROWS - 1) % ROWS][(j + 1) % COLS] +
                        old[i][(j + COLS - 1) % COLS] +
                        old[i][(j + 1) % COLS] +
                        old[(i + 1) % ROWS][(j + COLS - 1) % COLS] +
                        old[(i + 1) % ROWS][j] +
                        old[(i + 1) % ROWS][(j + 1) % COLS];

                  switch(count)
                  {
                  case 0:
                  case 1:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                  case 8:
                        new[i][j] = 0;
                        break;

                  case 2:
                        new[i][j] = old[i][j];
                        break;

                  case 3:
                        new[i][j] = 1;
                        break;
                  }

                  VioSetCurPos(i+1, j+1,0);
                  putch(new[i][j] ? '*' : ' ');
            }
      }
}

void initialize(void)
{
      int i, j;

      randomize();
      for (i = 0; i < ROWS; ++i)
      {
            for (j = 0; j < COLS; ++j)
            {
                  civ1[i][j] = random(2);
                  VioSetCurPos(i+1, j+1,0);
                  putch(civ1[i][j] ? '*' : ' ');
            }
      }
}

int main(void)
{
      int i;

      initialize();
      for (i = 0; i < GENERATIONS; ++i)
      {
            update_generation(civ1, civ2);
            update_generation(civ2, civ1);
      }
      return EXIT_SUCCESS;
}

