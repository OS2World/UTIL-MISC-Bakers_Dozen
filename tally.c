/* $title:      'TALLY.C ===== Count text file words & lines =====' */
/* $subtitle:   'Elipse Ltd.  [jms]           Revised: 2008 May 05' */

/* NOTE: For wildcard expansion to work from the command line, */
/*       Link with SETARGV.OBJ                                 */

#define INCL_DOSPROCESS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#ifndef __OS2__
#define DCWW_NOWAIT  0
#define DCWW_WAIT    1
typedef unsigned long TID;
#define DosEnterCritSec() 0
#define DosExitCritSec()  0
#define DosWaitThread(x, y )
#define _beginthread(fn, stp, sts, arg )   0; fn( arg )
#else
#include <os2.h>
#endif

#define LINESPERPAGE    60

struct tallydata
    {
    TID proc;
    char *filename;
    long bytes;
    long text;
    long words;
    long lines;
    long pages;
    struct tallydata *next;
    };

static long tot_bytes = 0L;             /* total character count */
static long tot_text = 0L;              /* total text characters */
static long tot_words = 0L;             /* total words */
static long tot_lines = 0L;             /* total number of lines */
static long tot_pages = 0L;             /* total number of pages */
static long num_files = 0L;             /* total number of files analysed */

int main(int argc, char *argv[]);
void tally(void *argstr);
void usage(void);
void count(FILE *input, long *bytes, long *text, long *words, long *lines, long *pages);
void total_up(const struct tallydata *curr);
void summary(void);


/* MAIN ===== Starting point ===== $pageif:6 */

int main(int argc, char *argv[])
    {                                   /* main */
    struct tallydata *head, *ptr;
    int i;
    fprintf( stderr, "\n\n\t TALLY\t---\t (C) Elipse Ltd.  May 2008\n\n" );
    if ( argc < 2 )
        usage( );

    head = NULL;
    for ( i = 1;  i < argc;  i++ )
        {
        ptr = malloc( sizeof(struct tallydata ) );
        memset( ptr, '\0', sizeof(struct tallydata ) );
        ptr->next = head;
        head = ptr;
        ptr->filename = argv[i];
        ptr->proc = _beginthread(tally, NULL, 12288, ptr );
        }

    tot_bytes = tot_text = tot_words = tot_lines = tot_pages = num_files = 0L;
    while ( head != NULL )
        {
        DosWaitThread(&head->proc, DCWW_WAIT );
        total_up( head );
        ptr = head->next;
        free( head );
        head = ptr;
        }

    if ( num_files > 1 )
        summary( );
    printf( "\n" );

    return 0;
    }                                   /* main */


/* USAGE ===== Input file name and open ===== $pageif:6 */

void usage(void)
    {                                   /* usage */
    fprintf( stderr, "\n\t USAGE:  tally filename [filename] ...\n" );
    abort( );
    }                                   /* usage */


/* TALLY ===== Main counting routine ===== $pageif:6 */

void tally(void *voidptr)
    {                                   /* tally */
    struct tallydata *ptr = voidptr;
    FILE *fp = NULL;
    char *infil = NULL;

    if ( ptr != NULL )
        {
        infil = ptr->filename;
        if ( infil != NULL )
            {
            fp = fopen( infil, "rb" );
            if ( fp != NULL )
                {
                count( fp, &ptr->bytes, &ptr->text, &ptr->words, &ptr->lines, &ptr->pages );
                fclose( fp );
                }
            }
        }
    }                                   /* tally */


/* COUNT ===== Main counting routine ===== $pageif:6 */

void count(FILE *input, long *bytes, long *text, long *words, long *lines, long *pages)
    {                                   /* count */
    int currline = 0;                   /* line number on current page */
    int prev = 1;                       /* prev char status (space) */
    int curr = 1;                       /* current char status (space) */
    int ch = 0;                         /* current character */
    long space = 0L;                    /* white space characters */
    long trans = 0L;                    /* transitions (words x 2) */
    *bytes = 0L;                        /* character count */
    *text = 0L;                         /* number of text characters */
    *words = 0L;                        /* number of words */
    *lines = 0L;                        /* number of lines */
    *pages = 0L;                        /* number of pages */

    while ( (ch = getc( input )) != EOF )
        {
        (*bytes)++;                     /* increment char count */
        if ( ch == '\f' )               /* form feed */
            {
            (*pages)++;                 /* increment page count */
            currline = 0;
            }
        if ( ch == '\n' )               /* new line */
            {
            (*lines)++;                 /* increment line count for file */
            currline++;                 /* increment line count for page */
            if ( currline > LINESPERPAGE )
                {
                (*pages)++;             /* increment page count */
                currline = 0;
                }
            }
        curr = isspace( ch );           /* set status */
        if ( curr != prev )
            {
            trans++;                    /* word transition */
            prev = curr;                /* remember for later */
            }
        if ( curr )
            space++;                    /* increment white space count */
        }

    (*lines)++;
    if ( !prev )
        trans++;
    (*words) = trans / 2L;
    (*text) = (*bytes) - space;
    if ( currline > 0 )
        (*pages)++;                     /* increment page count */
    }                                   /* count */


/* TOTAL_UP ===== Add current structure to totals ===== $pageif:6 */

void total_up(const struct tallydata *curr)
    {                                   /* total_up */
    printf( "\nFor %s ...\n", curr->filename );
    printf( "%9ld   all characters\n", curr->bytes );
    printf( "%9ld   text characters\n", curr->text );
    printf( "%9ld   words\n", curr->words );
    printf( "%9ld   lines\n", curr->lines );
    printf( "%9ld   pages\n", curr->pages );
    tot_bytes += curr->bytes;
    tot_text += curr->text;
    tot_words += curr->words;
    tot_lines += curr->lines;
    tot_pages += curr->pages;
    num_files++;
    }                                   /* total_up */


/* SUMMARY ===== Show totals and averages ===== $pageif:6 */

void summary(void)
    {                                   /* summary */
    printf( "\n\nSummary of %ld files ...\n", num_files );
    printf( "\n    TOTAL %19s AVERAGE \n\n", " " );
    printf( "%9ld   all  characters %9ld\n", tot_bytes, tot_bytes / num_files );
    printf( "%9ld   text characters %9ld\n", tot_text, tot_bytes / num_files );
    printf( "%9ld   .... words .... %9ld\n", tot_words, tot_words / num_files );
    printf( "%9ld   .... lines .... %9ld\n", tot_lines, tot_lines / num_files );
    printf( "%9ld   .... pages .... %9ld\n", tot_pages, tot_pages / num_files );
    }                                   /* summary */
