/* $title:      'PROTO.C ===== Extract prototypes from source =====' */
/* $subtitle:   'Elipse Ltd.  [jms]            Revised: 1994 Mar 04' */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFERSIZE      512
#define DEFBUFSIZE      BUFFERSIZE*8


int main( int argc, const char *argv[] );
FILE *open_input( char *name );
void extract_root( const char *name );
void proto( void );
int read_line( char *buf, int num );
void trim( char *buf );
int keep( char *buf );
void print_comment( char *buf );
int special( const char *buf );
void process_def( char *buf );
char *isolate_word( char *buf );
char *type_of( char *list, char *word );
char *find_word( const char *list, const char *word );
int val_var_chr( int ch );

static char root[128];
static FILE *fin = NULL;


/* MAIN ===== Entry point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    char name[256];
    fprintf( stderr, "\n\tPROTO: Output function prototypes.  (C) Elipse Ltd. Mar, 1994\n" );
    memset( name, '\0', sizeof (name) );
    memset( root, '\0', sizeof (root) );
    if ( argc > 1 )
        strcpy( name, argv[1] );
    fin = open_input( name );
    if ( fin == NULL )
        {
        fprintf( stderr, "\n\tError opening file\n" );
        return 1;
        }
    proto( );
    fclose( fin );
    fprintf( stderr, "\n\tEnd of program proto\n\n" );
    return 0;
    }                                   /* main */


/* OPEN_INPUT ===== Open file ===== $pageif:6 */

FILE *open_input( char *name )
    {                                   /* open_input */
    if ( *name == '\0' )
        {                               /* no name given */
        fprintf( stderr, "\n\tEnter source file name: " );
        fflush( stderr );
        gets( name );                   /* from keyboard */
        }
    strupr( name );
    if ( strchr( name, '.' ) == NULL )
        strcat( name, ".C" );           /* default to C program */

    fprintf( stderr, "\n\tSource file: %s\n\n", name );
    extract_root( name );
    return fopen( name, "r" );          /* now open it */
    }                                   /* open_input */


/* EXTRACT_ROOT ===== From full file name ===== $pageif:6 */

void extract_root( const char *name )
    {                                   /* extract_root */
    char temp[256], *p;
    memset( temp, '\0', sizeof (temp) );
    p = strchr( name, ':' );
    if ( p != NULL )
        strcpy( temp, p + 1 );
    else
        strcpy( temp, name );
    p = strchr( temp, '.' );
    if ( p != NULL )
        *p = '\0';
    p = strrchr( temp, '\\' );
    if ( p != NULL )
        strcpy( root, p + 1 );
    else
        strcpy( root, temp );
    }                                   /* extract_root */


/* PROTO ===== Body of program ===== $pageif:6 */

void proto( void )
    {                                   /* proto */
    char curdef[DEFBUFSIZE], buf[BUFFERSIZE];
    int first = 1;
    int indef = 0;
    memset( curdef, '\0', sizeof (curdef) );    /* initialize */
    while ( read_line( buf, sizeof (buf) ) )    /* from file */
        {
        if ( !keep( buf ) )             /* check if useful or not */
            indef = 0;
        else
            {
            if ( first )
                {
                printf( "\n#ifndef %s_H\n", root );
                printf( "#define %s_H 1\n\n", root );
                first = 0;
                }
            trim( buf );                /* comments and excess blanks out */
            if ( indef )
                strcat( curdef, buf );  /* add to buffer */
            else
                {
                process_def( curdef );  /* now show it all */
                strcpy( curdef, buf );  /* begin next one */
                indef = 1;
                }
            }
        }
    process_def( curdef );              /* process final buffer */
    printf( "\n#endif\n\n" );
    }                                   /* proto */


/* READ_LINE ===== From disk ===== $pageif:6 */

int read_line( char *buf, int num )
    {                                   /* read_line */
    char *p;
    if ( fgets( buf, num, fin ) == NULL )
        return 0;                       /* end of file */

    p = strchr( buf, '\n' );
    if ( p != NULL )
        (*p) = '\0';
    return 1;                           /* good data */
    }                                   /* read_line */


/* TRIM ===== Remove comments ===== $pageif:6 */

void trim( char *buf )
    {                                   /* trim */
    char *p, *q;
    while ( (p = strchr( buf, '/' )) != NULL )
        {
        if ( *(p + 1) == '/' )          /* C++ type comment */
            (*p) = '\0';
        if ( *(p + 1) == '*' )          /* normal comment start */
            {
            if ( (q = strstr( p + 2, "*/" )) == NULL )
                (*p) = '\0';
            else
                strcpy( p, q + 2 );
            }
        if ( (p > buf) && (*(p - 1) == '*') )   /* normal comment end */
            strcpy( buf, p + 1 );
        }
    for ( q = buf,  p = buf - 1;  *q;  ++q )
        if ( !isspace( *q ) )
            p = q;
    *(++p) = '\0';
    }                                   /* trim */


/* KEEP ===== Check if line is useful ===== $pageif:6 */

int keep( char *buf )
    {                                   /* keep */
    static int indir = 0;
    if ( *buf == '\0' )                 /* nothing */
        return 0;
    if ( *buf == '/' )                  /* comment */
        {
        if ( special( buf ) )           /* show line if */
            print_comment( buf );       /* title or subtitle */
        indir = 0;
        return 0;
        }
    if ( *buf == '#' )                  /* directive */
        {
        if ( !indir )                   /* replace continuous directives */
            printf( "\n" );             /* with single blank line */
        indir = 1;
        return 0;
        }
    if ( isspace( *buf ) )              /* function body or blank line */
        {
        indir = 0;
        return 0;
        }
    indir = 0;
    return 1;
    }                                   /* keep */


/* print_comment ===== With replacement ===== $pagef:6 */

void print_comment( char *buf )
    {                                   /* print_comment */
    char *p;
    p = strstr( buf, ".c" );
    if ( p != NULL )
        *(p + 1) = 'h';
    p = strstr( buf, ".C" );
    if ( p != NULL )
        *(p + 1) = 'H';
    printf( "%s\n", buf );
    }                                   /* print_comment */



/* SPECIAL ===== Check for special line ===== $pageif:6 */

int special( const char *buf )
    {                                   /* special */
    static const char *specials[] = {"$title:", "$subtitle:",
                              "$TITLE:", "$SUBTITLE:", NULL};
    int i = 0;
    while ( specials[i] != NULL )       /* return true if input buffer ...*/
        {                               /* ...contains one of these */
        if ( strstr( buf, specials[i] ) != NULL )
            return 1;
        i++;
        }
    return 0;
    }                                   /* special */


/* PROCESS_DEF ===== Combine and output ===== $pageif:6 */

void process_def( char *buf )
    {                                   /* process_def */
    char *p, *q, *b1, *b2, sep = '(';
    int isvoid = 1;
    int iscomplete = 0;
    if ( (b1 = strchr( buf, '(' )) == NULL )
        return;                         /* no function */
    if ( (b2 = strchr( b1, ')' )) == NULL )
        return;                         /* no function */
    if ( *(b2 + 1) == '\0' )
        iscomplete = 1;
    (*b1) = '\0';                       /* terminate here */
    printf( "%s", buf );                /* show function type and name only */
    b1++;                               /* point to remainder of string */

    for ( q = b1;  q < b2;  ++q )
        if ( !isspace( *q ) )           /* convert empty paren to void */
            isvoid = 0;
    if ( isvoid )
        {
        printf( "( void );\n" );
        return;                         /* no arguments */
        }

    if ( (p = strchr( b2, ';' )) != NULL )
        {                               /* check for imbedded type definition */
        iscomplete = 1;
        for ( q = b2 + 1;  q < p;  ++q )
            if ( !isspace( *q ) )       /* if ); combination then complete */
                iscomplete = 0;
        }

    if ( iscomplete )
        {
        if ( p != NULL )
            (*p) = '\0';                /* terminate */
        printf( "(%s;\n", b1 );         /* show it */
        if ( p != NULL )
            process_def( p + 1 );       /* search remainder of line */
        return;
        }

    while ( (b1 != NULL) && (b1 < b2) )
        {                               /* loop through parameters */
        p = isolate_word( b1 );
        printf( "%c %s", sep, type_of( b2, p ) );   /* show with type */
        sep = ',';
        b1 = strchr( b1 + 1, ',' );     /* search next parameter */
        }
    printf( " );\n" );                  /* final paren and semicolon */
    }                                   /* process_def */


/* ISOLATE_WORD ===== Copy single word to holding area ===== $pageif:6 */

char *isolate_word( char *buf )
    {                                   /* isolate_word */
    static char word[40];
    char *p = word;
    memset( word, '\0', sizeof (word) );
    while ( !val_var_chr( *buf ) && (*buf != '*') && (*buf != '[') && (*buf != ']') )
        buf++;
    while ( val_var_chr( *buf ) || (*buf == '*') || (*buf == '[') || (*buf == ']') )
        *p++ = *buf++;
    return word;
    }                                   /* isolate_word */


/* TYPE_OF ===== Given word ===== $pageif:6 */

char *type_of( char *list, char *word )
    {                                   /* type_of */
    static char type[BUFFERSIZE];
    char *p, *b, *e;
    memset( type, '\0', sizeof (type) );
    if ( *word == '\0' )
        {
        strcpy( type, " void " );
        return type;                    /* void if no word given */
        }
    if ( (p = find_word( list, word )) == NULL )
        {
        strcpy( type, " int " );
        strcat( type, word );
        return type;                    /* default if not found */
        }
    while ( (p >= list) && (!isspace( *p )) )
        p--;                            /* back up to beginning of word */
    p++;
    for ( b = p;  b >= list;  --b )     /* back up to beginning of list */
        if ( (*b == ';') || (*b == ')') )
            break;
    b++;
    e = strchr( b, ',' );
    if ( (e == NULL) || (e > p) )
        e = p;
    while ( (e >= b) && (!isspace( *e )) )
        e--;                            /* back up to beginning of word */
    strncpy( type, b, (unsigned) (e - b + 1) );
    strcat( type, isolate_word( p ) );
    return type;
    }                                   /* type_of */


/* FIND_WORD ===== In given list ===== $pageif:6 */

char *find_word( const char *list, const char *word )
    {                                   /* find_word */
    char *p = strstr( list, word );
    char *q = p + strlen( word );
    if ( p == NULL )
        return NULL;                    /* not there */
    if ( (p > list) && (val_var_chr( *(p - 1) )) )
        return find_word( q, word );
    if ( val_var_chr( *(q) ) )
        return find_word( q, word );
    return p;
    }                                   /* find_word */


/* VAL_VAR_CHR ===== Return true if alpha, digit or '_' ===== $pageif:6 */

int val_var_chr( int ch )
    {                                   /* val_var_chr */
    return (isalnum( ch ) || (ch == '_'));
    }                                   /* val_var_chr */
