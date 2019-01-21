/* $title:      'ADJTEXT.C ===== Flow text to fit given width =====' */
/* $subtitle:   'Elipse Ltd.  [jms]            Revised: 1993 Jan 03' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "adjfile.h"
#include "hyphen.h"

int main( int argc, const char *argv[] );
void adjtext( void );
char *break_up( char *word, unsigned lgth, unsigned excess );
int choose_width( int w );
int choose_file( const char *name );
char *parse_word( char *databuf );
int next_word( char *currword );

static int lwidth;                      /* desired output width */
static unsigned nlines = 0;             /* total number of lines */
static unsigned nwords = 0;             /* total number of words */


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    char filnam[256];
    int n = 0;
    lwidth = 0;
    memset( filnam, '\0', sizeof (filnam) );
    fprintf( stderr, "\n\n\tAdjText\t---\t(C) Elipse Ltd.  Jan, 1993\n\n" );

    if ( argc > 1 )
        strcpy( filnam, argv[1] );
    if ( argc > 2 )
        n = atoi( argv[2] );
    lwidth = choose_width( n );
    if ( lwidth == 0 )
        return 1;
    if ( !choose_file( filnam ) )
        return 2;

    adjtext( );
    fprintf( stderr, "\n \t AdjText complete \n" );
    fprintf( stderr, "\n \t Totals:  lines = %u, words = %u\n", nlines, nwords );
    return 0;
    }                                   /* main */


/* ADJTEXT ===== Body of program ===== $pageif:6 */

void adjtext( void )
    {                                   /* adjtext */
    char word[BUFSIZE];
    int type, sent = 0;
    unsigned col, lgth;
    nlines = nwords = col = 0;
    memset( word, '\0', sizeof (word) );

    while ( 1 )
        {
        type = next_word( word );
        lgth = strlen( word );
        switch ( type)
            {
        case ENDFILE:                   /* final return and quit */
            printf( "\n" );
            nlines++;
            return;
            break;
        case NEWLINE:                   /* hard return before word */
            col = lgth;
            if ( lgth > 0 )
                nwords++;
            printf( "\n%s", word );
            nlines++;
            break;
        case NORMAL:                    /* nothing special */
            col += (lgth + 1);
            nwords++;
            if ( col <= lwidth )
                printf( " %s", word );
            else
                {                       /* soft return */
                char *nxtpart = break_up( word, lgth, col - lwidth );
                if ( sent && (nxtpart[0] == ' ') && (nxtpart[1] != ' ') )
                    ++nxtpart;
                col = strlen( nxtpart );
                if ( word[0] != '\0' )
                    printf( " %s-\n%s", word, nxtpart );
                else
                    printf( "\n%s", nxtpart );
                nlines++;
                }
            break;
            }
        if ( lgth > 0 )
            sent = (strchr( ".?!", word[lgth - 1] ) != NULL);
        else
            sent = 0;
        }
    }                                   /* adjtext */


/* BREAK_UP ===== Check hyphenation possibility ===== $pageif:6 */

char *break_up( char *word, unsigned lgth, unsigned excess )
    {                                   /* break_up */
    static char remainder[BUFSIZE];
    ctype *beg = (ctype *) (word);
    ctype *end = (ctype *) (word + lgth - 1);
    unsigned aim = lgth - excess - 2;   /* one for blank & one for hyphen */
    unsigned spot = 0;
    unsigned i;

    memset( remainder, '\0', sizeof (remainder) );
    if ( (aim > 1) && (aim <= lgth) )   /* is it feasable */
        {
        while ( (beg < end) && !isalpha( *beg ) )   /* trim initial junk */
            ++beg;
        while ( (beg < end) && !isalpha( *end ) )   /* trim final junk */
            --end;
        if ( hyphen( beg, end ) )       /* try it */
            {
            for ( i = 0;  i < lgth;  ++i )
                {
                if ( (unsigned char) word[i] > 0x7f )
                    {
                    word[i] &= 0x7f;    /* remove high bit markers */
                    if ( (i > 1) && (i <= aim) )
                        spot = i;       /* note suitable positions */
                    }
                }
            }
        }
    strcpy( remainder, &word[spot] );   /* second half */
    word[spot] = '\0';                  /* first half */
    return remainder;
    }                                   /* break_up */



/* CHOOSE_WIDTH ===== Manual input of text width ===== $pageif:6 */

int choose_width( int w )
    {                                   /* choose_width */
    if ( (w < MAXWORD) || (w > BUFSIZE / 2) )
        {
        char ans[256];
        fprintf( stderr, "\n\n Enter text width: " );
        fflush( stderr );
        w = atoi( gets( ans ) );
        fprintf( stderr, "\n\n" );
        }
    if ( (w < MAXWORD) || (w > BUFSIZ / 2) )
        {
        fprintf( stderr, "\n Text width %d is not allowed.\n", w );
        return 0;
        }
    return w;
    }                                   /* choose_width */


/* CHOOSE_FILE ===== Input file name and open ===== $pageif:6 */

int choose_file( const char *name )
    {                                   /* choose_file */
    char ans[256];
    int ret = 0;
    strcpy( ans, name );
    if ( strlen( ans ) > 0 )
        ret = file_open( ans, lwidth );
    if ( !ret )
        {
        fprintf( stderr, "\n\n Enter input file name: " );
        fflush( stderr );
        gets( ans );
        fprintf( stderr, "\n\n" );
        ret = file_open( ans, lwidth );
        }
    if ( !ret )
        fprintf( stderr, "\n Cannot open file %s.\n", ans );
    return ret;
    }                                   /* choose_file */


/* PARSE_WORD ===== Locate end of first word in string ===== $pageif:6 */

char *parse_word( char *databuf )
    {                                   /* parse_word */
    while ( *databuf == ' ' )
        ++databuf;
    return strchr( databuf, ' ' );
    }                                   /* parse_word */


/* NEXT_WORD ===== Return pointer to next whole word ===== $pageif:6 */

int next_word( char *currword )
    {                                   /* next_word */
    static char currline[BUFSIZE];
    static int currtype = -1;
    static int prevtype = NIL;
    static int linefeed = 0;
    char *endword;

    *currword = '\0';                   /* clear return word */
    if ( currtype < 0 )                 /* if first time in routine */
        currtype = file_read( currline );   /* read first line */

    if ( *currline == '\0' )
        {
        linefeed = 0;
        prevtype = currtype;
        currtype = file_read( currline );   /* read another line */
        if ( currtype == ENDFILE )
            return ENDFILE;
        if ( (currtype == SPACE) || (currtype == TABS) || (currtype == NIL) )
            linefeed = 1;
        if ( (prevtype != NORMAL) && (currtype == SHORT) )
            linefeed = 1;
        return NEWLINE;
        }

    endword = parse_word( currline );   /* locate end of word */
    if ( endword != NULL )
        {
        *endword = '\0';                /* replace space */
        strcpy( currword, currline );   /* copy word to return */
        strcpy( currline, ++endword );  /* shift current line */
        if ( linefeed )
            {
            linefeed = 0;
            return NEWLINE;
            }
        else
            return NORMAL;
        }
    else
        {                               /* last word in line */
        strcpy( currword, currline );   /* remainder of line */
        prevtype = currtype;            /* remember previous */
        currtype = file_read( currline );   /* get another line */
        endword = currword + strlen( currword ) - 1;
        if ( (currtype != SPACE) && (currtype != ENDFILE) && (*endword == '-') )
            {                           /* hyphenated word, so join */
            *endword = '\0';            /* remove hyphen */
            endword = parse_word( currline );
            if ( endword != NULL )      /* just one word */
                {
                *endword = '\0';        /* replace space */
                strcat( currword, currline );   /* cat word to return */
                strcpy( currline, ++endword );  /* shift current line */
                }
            else
                {
                strcat( currword, currline );   /* cat word to return */
                prevtype = currtype;    /* remember previous */
                currtype = file_read( currline );   /* get another line */
                }
            return NORMAL;
            }
        else
            {
            if ( (currtype == SPACE) || (currtype == TABS) || (currtype == NIL) )
                linefeed = 1;
            if ( (prevtype != NORMAL) && (currtype == SHORT) )
                linefeed = 1;
            return NORMAL;
            }
        }
    }                                   /* next_word */

