/* $title:      'SDF2WP.C ========== File Filter ==========' */
/* $subtitle:   'Elipse Ltd.  [jms]    Revised: 1995 Dec 04' */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXRECLEN   4096

int main( int argc, char *argv[] );
void sdf2wp( void );
char *parse( char *buf );
char *pull_norm( char * buf );
char *pull_quote( char *buf );
char *strtrm( char *str );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\n SDF2WP ===== Convert Comma Delimited Records\n" );
    fprintf( stderr, "                to WordPerfect Mail Merges\n" );
    fprintf( stderr, "                Elipse Ltd. December, 1995\n\n" );
    sdf2wp( );                          /* do conversion */
    return 0;
    }                                   /* main */


/* SDF2WP ===== Body of program ===== $pageif:6 */

void sdf2wp( void )
    {                                   /* sdf2wp */
    char buf[MAXRECLEN + 4];
    memset( buf, '\0', sizeof (buf) );              /* start empty */

    while ( fgets( buf, MAXRECLEN, stdin ) != NULL )
        {
        char *field = parse( buf );                 /* get first field */
        while ( field != NULL )                     /* repeat while data */
            {
            printf( "%s\x12", strtrm( field ) );    /* Field and (^R) */
            field = parse( NULL );                  /* get next field */
            }
        printf( "\x05\n" );                         /* End record (^E) */
        }
    }                                   /* sdf2wp */


/* PARSE ===== Return array of fields ===== $pageif:6 */

char *parse( char *str )
    {                                   /* parse */
    static char buffer[ MAXRECLEN + 8], *curpos;
    char *fieldpos;

    if ( str == NULL )
        {                                   /* continue current data */
        if ( curpos == NULL )
            return NULL;                    /* no more data in buffer */
        }
    else
        {                                   /* fill buffer with new data */
        strncpy( buffer, str, sizeof (buffer) );
        curpos = buffer;                    /* point to start */
        }

    fieldpos = curpos;                      /* field begins at current */
    if ( *fieldpos == '\"' )
        curpos = pull_quote( ++fieldpos );  /* quoted field; remove quotes */
    else
        curpos = pull_norm( fieldpos );     /* simple parse */

    return fieldpos;                        /* start of field */
    }                                   /* parse */


/* PULL_NORM ===== Isolate field and move pointer ===== $pageif:6 */

char *pull_norm( char *buf )
    {                                   /* pull_norm */
    if ( (buf != NULL) && (*buf != '\0') )
        {
        char *p;
        p = strchr( buf, ',' );         /* look for separator */
        if ( p != NULL )
            {
            *p = '\0';                  /* terminate field string */
            return (p + 1);             /* return start of next field */
            }
        p = strchr( buf, '\n' );        /* look for end of input string */
        if ( p != NULL )
            {
            *p = '\0';                  /* terminate field string */
            return NULL;                /* show as last field */
            }
        }
    return NULL;                        /* nothing to do */
    }                                   /* pull_norm */


/* PULL_QUOTE ===== Isolate field and move pointer ===== $pageif:6 */

char *pull_quote( char *buf )
    {                                   /* pull_quote */
    if ( (buf != NULL) && (*buf != '\0') )
        {
        while ( 1 )
            {
            char *q, *p;
            p = strchr( buf, '\"' );        /* find next quote */
            if ( p == NULL )
                return pull_norm( buf );    /* if none, treat as non-quoted */
            q = p + 1;
            switch ( *q)                    /* check character following quote */
                {
            case ',':                       /* separator, so end of field */
                *p = '\0';                  /* terminate field string */
                return (q + 1);             /* return start of next field */
                break;
            case '\n':                      /* new line, so end of string */
                *p = '\0';
            case '\0':                      /* nul, so end of string */
                return NULL;                /* show as last field */
            case '\"':                      /* double quote */
                *p = ' ';                   /* remove one of the pair */
            default:
                buf = q;                    /* keep looking */
                }
            }
        }
    return NULL;                            /* nothing to do */
    }                                   /* pull_quote */


/* STRTRM ===== Trim blanks on the right of string ===== $pageif:6 */

char *strtrm( char *str )
    {                                   /* strtrm */
    char *j, *p;
    for ( j = str,  p = str - 1;  *j;  ++j )
        if ( !isspace( *j ) )           /* Find last non-space */
            p = j;
    *(++p) = '\0';                      /* and place zero there */
    return str;
    }                                   /* strtrm */
