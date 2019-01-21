/* $title:      'CASH.C ===== Print cash flow from input file =====' */
/* $subtitle:   'Elipse Ltd.  [jms]            Revised: 1995 Dec 27' */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct statistics
    {
    short month;
    short day;
    short num_ent;
    double bal_low;
    double bal_high;
    double bal_prev;
    double bal_sum;
    double tot_in;
    double tot_out;
    struct statistics *next;
    } STATS;

int main( int argc, const char *argv[] );
void cash( const char *filename );
FILE *open_input( const char *name );
int parse_next_line( FILE *inf, int *day, char *desc, double *amt);
int get_description( char *des, const char *raw );
STATS *alloc_stats( int mth, STATS *tail );
void accum_stats( STATS *ptr, int day, double amt, double bal );
void print_all_stats( STATS *head, const char *name );
void print_month_stats( STATS *ptr );
char *string_trim( char *str );
const char *month_name( int mth );
int month_days( int mth );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    int i;
    fprintf( stderr, "\nCASH ===== (C) Elipse Ltd. %s\n", __DATE__ );
    if ( argc < 2 )
        {
        fprintf( stderr, "\nReguire the name of the input file(s)\n" );
        return 1;
        }
    for ( i = 1;  i < argc;  ++i )
        cash( argv[i] );
    return 0;
    }                                   /* main */


/* CASH ===== Body of program ===== $pageif:6 */

void cash( const char *filename )
    {                                   /* cash */
    const static char *pfmt = "%3d  %-32s %10.2f            %11.2f\n";
    const static char *nfmt = "%3d  %-32s            %10.2f %11.2f\n";
    const static char *zfmt = "%3d  %-32s                       %11.2f\n";
    const static char *hfmt = "\n\n%3s  %-32s %10s %10s %11s\n\n";
    STATS head, *tail = &head;
    int rval = 0;
    int day = 0;
    double amt = 0.0;
    double bal = 0.0;
    char desc[128];
    FILE *inf = open_input( filename );
    if ( inf == NULL )
        return;
    memset( desc, '\0', sizeof (desc) );
    memset( &head, '\0', sizeof (head) );
    printf( "\n     %-32s  %32s\n", "Cash Flow Detail", filename );
    while ( (rval = parse_next_line( inf, &day, desc, &amt )) >= 0 )
        {
        if ( rval > 0 )
            {
            printf( hfmt, month_name( rval ), "Description", "Amt(+)", "Amt(-)", "Balance" );
            tail = alloc_stats( rval, tail );
            }
        else
            {
            bal += amt;
            if ( amt < -0.004 )
                printf( nfmt, day, desc, -amt, bal );   /* print negative */
            else if ( amt > +0.004 )
                printf( pfmt, day, desc, +amt, bal );   /* print positive */
            else
                printf( zfmt, day, desc, bal );         /* print zero */
            accum_stats( tail, day, amt, bal );
            }
        }
    print_all_stats( head.next, filename );
    }                                   /* cash */


/* OPEN_INPUT ===== File to read from ===== $pageif:6 */

FILE *open_input( const char *name )
    {                                   /* open_input */
    FILE *fp = NULL;
    char fbuf[260];
    memset( fbuf, '\0', sizeof (fbuf) );
    strcpy( fbuf, name );
    fp = fopen( fbuf, "r" );
    if ( fp != NULL )
        return fp;
    strcat( fbuf, ".csh" );
    fp = fopen( fbuf, "r" );
    if ( fp != NULL )
        return fp;
    fprintf( stderr, "\nUnable to open input file %s\n", name );
    return NULL;
    }                                   /* open_input */


/* PARSE_NEXT_LINE ===== From input file ===== $pageif:6 */

int parse_next_line( FILE *inf, int *day, char *desc, double *amt)
    {                                   /* parse_next_line */
    int rval = -1;
    char buf[256], *p, *q;
    if ( inf == NULL )
        return -2;
    memset( buf, '\0', sizeof (buf) );
    *day = 0;
    *desc = '\0';
    *amt = 0.0;

    do  
        {
        if ( fgets( buf, sizeof (buf), inf ) == NULL )
            break;
        }
    while ( (p = strchr( buf, ',' )) == NULL );

    if ( p != NULL )
        {
        *p++ = '\0';
        q = strchr( p, ',' );
        *day = atoi( buf );
        if ( q != NULL )
            {
            *q++ = '\0';
            *amt = atof( q );
            }
        rval = get_description( desc, p );
        }

    if ( rval < 0 )
        {
        fclose( inf );
        inf = NULL;
        return -1;
        }
    if ( rval > 0 )
        return (*day);
    return 0;
    }                                   /* parse_next_line */


/* GET_DESCRIPTION ===== From raw input ===== $pageif:6 */

int get_description( char *des, const char *raw )
    {                                   /* get_description */
    while ( *raw == ' ' )
        ++raw;
    strcpy( des, raw );
    string_trim( des );
    if ( stricmp( des, "END" ) == 0 )
        return -1;                      /* signal end of file */
    if ( stricmp( des, "MONTH" ) == 0 )
        return 1;                       /* signal new month */
    return 0;                           /* normal description */
    }                                   /* get_description */


/* ALLOC_STATS ===== Memory for statistics ===== $pageif:6 */

STATS *alloc_stats( int mth, STATS *tail )
    {                                   /* alloc_stats */
    STATS *new = malloc( sizeof( STATS ) );
    if ( new == NULL )
        {
        fprintf( stderr, "\nOut of memory in malloc\n" );
        }
    else
        {
        new->month = (short) mth;
        new->day = 0;
        new->num_ent = 0;
        new->bal_low = +1e30;
        new->bal_high = -1e30;
        new->bal_prev = 0.0;
        new->bal_sum = 0.0;
        new->tot_in = 0.0;
        new->tot_out = 0.0;
        new->next = NULL;
        if ( tail != NULL )
            tail->next = new;
        }
    return new;
    }                                   /* alloc_stats */


/* ACCUM_STATS ===== Into structure ===== $pageif:6 */

void accum_stats( STATS *ptr, int day, double amt, double bal )
    {                                   /* accum_stats */
    if ( ptr != NULL )
        {
        int delta_day = day - ptr->day;
        ptr->day = day;
        ptr->bal_sum += delta_day * ptr->bal_prev;
        if ( amt < 0.0 )
            ptr->tot_out += amt;
        else
            ptr->tot_in += amt;
        if ( bal < ptr->bal_low )
            ptr->bal_low = bal;
        if ( bal > ptr->bal_high )
            ptr->bal_high = bal;
        ptr->bal_prev = bal;
        ++ptr->num_ent;
        }
    }                                   /* accum_stats */


/* PRINT_ALL_STATS ===== From structure ===== $pageif:6 */

void print_all_stats( STATS *head, const char *name )
    {                                   /* print_all_stats */
    printf( "\n\n     %-32s  %32s\n\n", "Cash Flow Summary", name );
    printf( "\nMonth   Number    Amt(+)     Amt(-)      LowBal     HighBal      AvgBal\n\n" );

    while ( head != NULL )
        {
        STATS *nxt = head->next;
        print_month_stats( head );
        free( head );
        head = nxt;
        }
    printf( "\n" );
    }                                   /* print_all_stats */


/* PRINT_MONTH_STATS ===== From structure ===== $pageif:6 */

void print_month_stats( STATS *ptr )
    {                                   /* print_month_stats */
    if (ptr != NULL)
        {
        int mth = ptr->month;
        if (mth > 0)
            {
            int last_day = month_days( mth );
            if (last_day > 0)
                {
                int delta_day = last_day - ptr->day;
                ptr->bal_sum += delta_day * ptr->bal_prev;
                printf( "\n%4s %6d %11.0f %11.0f %11.0f %11.0f %11.0f\n",
                    month_name( ptr->month ), ptr->num_ent, ptr->tot_in, -ptr->tot_out,
                    ptr->bal_low, ptr->bal_high, (ptr->bal_sum / last_day) );
                }
            }
        }
    }                                   /* print_month_stats */


/* STRTRM ===== Trim blanks on the right of string ===== $pageif:6 */

char *string_trim( char *str )
    {                                   /* strtrm */
    char *j, *p;
    for ( j = str,  p = str - 1;  *j;  ++j )
        if ( !isspace( *j ) )           /* Find last non-space */
            p = j;
    *(++p) = '\0';                      /* and place zero there */
    return str;
    }                                   /* strtrm */


/* MONTH_NAME ===== Given number 1-12, return name ===== $pageif: 6 */

const char *month_name( int mth )
    {                                   /* month_name */
    const static char *allmo[] = { "???", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                   "JUL", "AUG", "SEP", "OCT", "NOV", "DEC", NULL };
    if ( (mth < 0) || (mth > 12) )
        mth = 0;
    return allmo[mth];
    }                                   /* month_name */


/* MONTH_DAYS ===== Return days in month ===== $pageif:6 */

int month_days( int mth )
    {                                   /* month_days */
    const static int allday[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0 };
    if ( (mth < 0) || (mth > 12) )
        mth = 0;
    return allday[mth];
    }                                   /* month_days */

