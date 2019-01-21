/* $title:      'VOLUME.C ===== Show volume label of given disk =====' */
/* $subtitle:   'Elipse Ltd.  [jms]              Revised: 2008 May 07' */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>

typedef struct {
    unsigned short  twosecs : 5;    /* seconds / 2 */
    unsigned short  minutes : 6;
    unsigned short  hours   : 5;
} ftime_t;

typedef struct {
    unsigned short  day     : 5;
    unsigned short  month   : 4;
    unsigned short  year    : 7;

} fdate_t;


int main( int argc, char *argv[] );
char *volume( char *spec );


/* MAIN ===== Starting point for program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {
    char *v = NULL;
    char spec[8];
    memset( spec, '\0', sizeof( spec ) );
    if (argc > 1)
        {
        spec[0] = toupper( *argv[1] );
        strcat( spec, ":\\*" );
        }

    v = volume( spec );
    if (v == NULL)
        {
        printf( "*No-Label*\n" );
        return 1;
        }
    else
        {
        printf( "%s\n", v );
        return 0;
        }
    }


/* VOLUME ===== Body of program ===== $pageif:6 */

char *volume( char *spec )
    {                                   /* volume */
    DIR *dirp;
    struct dirent *direntp;
    ftime_t *f_time;
    fdate_t *f_date;

    dirp = opendir( "\\" );
    if( dirp != NULL )
        {
        for(;;)
            {
            direntp = readdir( dirp );
            if( direntp == NULL ) break;
            f_time = (ftime_t *)&direntp->d_time;
            f_date = (fdate_t *)&direntp->d_date;

            printf( "%-12s %d/%2.2d/%2.2d "
                    "%2.2d:%2.2d:%2.2d \n",
                direntp->d_name,
                f_date->year + 1980,
                f_date->month,
                f_date->day,
                f_time->hours,
                f_time->minutes,
                f_time->twosecs * 2 );
            }
        closedir( dirp );
        }                                   /* volume */
    return NULL;
    }
