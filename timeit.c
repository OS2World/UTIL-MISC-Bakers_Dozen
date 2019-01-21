/* Program:  TIMEIT.C for OS/2 V1.0 Protected Mode and MS-DOS
 * Function: Time other commands
 * Written:  3/88 by Nico Mak
 * Format:   TIMEIT <command to time>
 * Notes:    Does not properly handle tasks that last over 24 hours.
 *           Tested under Microsoft SDK (2/88 release) and PC-DOS 3.3.
 * Sample Make file:
 *     timeit.exe:    timeit.c
 *       cl /AL /Oals timeit.c
 *       bind timeit.exe $(bindlib)\api.lib $(bindlib)\doscalls.lib
 */

#include <stdio.h>
#include <doscalls.h>   /* from 1Q88 Microsoft OS/2 SDK */

#define MAKE_LONG(hi, low) \
 ((long) ((((unsigned long) (unsigned) (hi)) << 16) + \
 (unsigned long) (unsigned) (low)))

long cvt();

main()
    {
    struct DateTime start_datetime;     /* starting date/time */
    struct DateTime end_datetime;       /* ending date/time */
    unsigned envpointer, cmdoffset;     /* parms to DOSGETENV */
    char *cmdline;                      /* pointer to command tail */
    long hh, mm, ss, hundredths;        /* portions of result times */

    /* point at command tail */
    DOSGETENV(&envpointer, &cmdoffset);
    cmdline = (char far *)MAKE_LONG(envpointer, cmdoffset);
    cmdline += strlen(cmdline) + 1;

    /* issue the command */
    printf(">>>Timing: %s\n", cmdline);
    DOSGETDATETIME(&start_datetime);
    system(cmdline);
    DOSGETDATETIME(&end_datetime);

    /* determine how long it took */
    hundredths = cvt(&end_datetime) - cvt(&start_datetime);
    if (start_datetime.day != end_datetime.day)
        hundredths += (24 * 60 * 60 * 100);
    hh = hundredths / (60 * 60 * 100);
    hundredths %=     (60 * 60 * 100);
    mm = hundredths / (60 * 100);
    hundredths %=     (60 * 100);
    ss = hundredths / 100;
    hundredths %=     100;
    printf(">>>Time was: %02ld:%02ld:%02ld.%02ld\n", hh, mm, ss, hundredths);
    }

/* cvt - given DateTime structure, return time in hundredths of seconds */
long cvt(dt)
 struct DateTime *dt;
    {
    return (((dt->hour * 60 * 60) + (dt->minutes * 60) + dt->seconds) * 100) +
     dt->hundredths;
    }
