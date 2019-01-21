/* title:       'APCAL.C  =====  Appointment Calendar Printout =====' */
/* subtitle:    'Elipse Ltd.  [jms]             Revised: 1993 Nov 11' */


#include <stdlib.h>
#include <stdio.h>

#define HIBYTE(c)       ((c) >> 8)
#define LOBYTE(c)       ((c) & 0xFF)
#define HILO(c1, c2)    ((c1) << 8) | (c2)
#define BOXHT           10
#define NUMWK           3
#define NREP            2


int main( int argc, char *argv[] );
int make_date( int m, int d);
void set_printer( void );
void reset_printer( void );
void draw_top( void );
void draw_bottom( void );
int draw_week( int moda, int ht );
int next_day( int moda );
void single_line( int c1, int c2, int c3, int c4, int sz, char *str[] );

static int mosz[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static FILE *printer = NULL;


/* MAIN ===== Print appointment calendar ===== pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int moda = 0;                       /* month & day combo. (one byte each) */
    int m = 0;                          /* month alone */
    int d = 0;                          /* day alone */
    int w = 0;                          /* week number */
    int r = 0;                          /* repeat count */

    if ( argc > 2 )
        {                               /* Command line ... */
        m = atoi( argv[1] );            /* First month */
        d = atoi( argv[2] );            /* Then day */
        if ( argc > 3 )
            mosz[1] = 29;               /* fix Feb if leap year */
        else
            mosz[1] = 28;               /* or normal if not */
        }
    while ( !moda )
        moda = make_date( m, d );       /* Combine to single int */
    set_printer( );                     /* Set font etc */

    for ( r = 1;  r <= NREP;  ++r )
        {
        draw_top( );                    /* Show days and lines */
        for ( w = 1;  w <= NUMWK;  ++w )    /* Do appropriate number of weeks */
            moda = draw_week( moda, BOXHT );
        draw_bottom( );                 /* Lower lines to finish */
        if ( r < NREP )
            fprintf( printer, "\n\n\n\n\n\n" );
        }

    reset_printer( );                   /* Back to normal */
    return 0;
    }                                   /* main */


/* MAKE_DATE ===== Combine month & day to single int ===== pageif:6 */

int make_date( int m, int d )
    {                                   /* make_date */

    printf( "\n Appointment calendar program  " );
    printf( "\t\t (C) Elipse Ltd.  Nov, 1993 \n" );
    while ( (m < 1) || (m > 12) || (d < 1) || (d > 31) )
        {
        printf( "\n Enter month and day (mm/dd) of the first sunday: " );
        fflush( stdout );
        scanf( "%d/%d", &m, &d );
        printf( "\n\n" );
        }
    return HILO((m-1), (d-1));
    }                                   /* make_date */


/* SET_PRINTER ===== Set up for box character printing ===== pageif:6 */

void set_printer( void )
    {                                   /* set_printer */
    static const char esc = '\x1b';
    printer = fopen( "PRN", "w" );      /* Open printer */
    if ( printer == NULL )
        {
        printf( "\nUnable to open printer!\n" );
        abort( );
        }
    fprintf( printer, "%cE", esc );     /* Reset printer */
    fprintf( printer, "%c&l1H", esc );  /* Upper tray */
    fprintf( printer, "%c&l0O", esc );  /* Portrait */
    fprintf( printer, "%c9", esc );     /* Clear margins */
    fprintf( printer, "%c&a1L", esc );  /* Left margin at 1 */
    fprintf( printer, "%c(10U", esc );  /* Symbol set PC-8 */
    fprintf( printer, "%c(s12H", esc ); /* 12 Pitch */
    fprintf( printer, "%c&l5.3333C", esc ); /* 9 lpi vertical */
    fprintf( printer, "\n\n\n" );       /* Begin on 4th line */
    }                                   /* set_printer */


/* RESET_PRINTER ===== Back to normal printing ===== pageif:6 */

void reset_printer( void )
    {                                   /* reset_printer */
    static const char esc = '\x1b';
    fprintf( printer, "\f" );           /* Form feed */
    fprintf( printer, "%cE", esc );     /* Reset printer */
    fprintf( printer, "%c&a6L", esc );  /* Left margin at 6 */
    fprintf( printer, "%c&l6D", esc );  /* 6 lpi vertical */
    fclose( printer );
    }                                   /* reset_printer */


/* DRAW_TOP ===== Draw upper section of calendar include days ===== pageif:6 */

void draw_top( void )
    {                                   /* draw_top */
    static char *days[7] = {"   Sun", "   Mon", "   Tue", "   Wed", 
        "   Thu", "   Fri", "   Sat"};
    single_line( 201, 209, 205, 187, 0, days ); /* upper line */
    single_line( 186, 179, 32, 186, 6, days );  /* print day names */
    }                                   /* draw_top */


/* DRAW_BOTTOM ===== Draw the lower section of calendar ===== pageif:6 */

void draw_bottom( void )
    {                                   /* draw_bottom */
    static char *blank[7] = {"", "", "", "", "", "", ""};
    single_line( 200, 207, 205, 188, 0, blank );    /* lower line */
    }                                   /* draw_bottom */


/* DRAW_WEEK ===== Do a single week showing date ===== pageif:6 */

int draw_week( int moda, int ht )
    {                                   /* draw_week */
    static char *mos[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static char *desc[7] = {"       ", "       ", "       ", "       ", 
        "       ", "       ", "       "};
    int i, mp, m, d;

    single_line( 199, 197, 196, 182, 0, desc ); /* upper line */
    mp = -1;
    for ( i = 0;  i <= 6;  ++i )
        {
        m = HIBYTE(moda);
        d = LOBYTE(moda) + 1;
        if ( m != mp )
            sprintf( desc[i], "%2d %3s", d, mos[m] );
        else
            sprintf( desc[i], "%2d    ", d );
        mp = m;
        moda = next_day( moda );
        }
    single_line( 186, 179, 32, 186, 6, desc );  /* print dates */

    for ( i = 1;  i <= ht;  ++i )
        single_line( 186, 179, 32, 186, 0, desc );  /* print body section */
    return moda;
    }                                   /* draw_week */


/* NEXT_DAY ===== Given date as moda, return the next day ===== pageif:6 */

int next_day( int moda )
    {                                   /* next_day */
    int m, d;
    m = HIBYTE(moda);                   /* seperate month */
    d = LOBYTE(moda);                   /* and day */
    if ( ++d < mosz[m] )                /* check month size */
        return ++moda;                  /* normal mid-month */
    if ( ++m >= 12 )                    /* increment month */
        m = 0;                          /* wrap around year */
    return HILO(m, 0);                  /* return new month */
    }                                   /* next_day */


/* SINGLE_LINE ===== Draw one line with seperators ===== pageif:6 */

void single_line( int c1, int c2, int c3, int c4, int sz, char *str[] )
    {                                   /* single line */
    static int wth[7] = {8, 14, 14, 14, 14, 14, 8};
    int i, j, k;
    char *s;
    putc( c1, printer );                /* opening corner */
    for ( i = 0;  i <= 6;  ++i )
        {
        k = c2;                         /* day seperator */
        s = str[i];
        while ( s < str[i] + sz )
            putc( *s++, printer );      /* string */
        for ( j = sz + 1;  j <= wth[i];  ++j )
            putc( c3, printer );         /* fill character */
        if ( (i == 6) && (j > wth[i]) )
            k = c4;                     /* closing corner */
        putc( k, printer );
        }
    putc( '\n', printer );
    }                                   /* single_line */
