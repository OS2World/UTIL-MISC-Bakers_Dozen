/* $title:      'TONEW.C ===== Create script to rename files =====' */
/* $subtitle:   'Elipse Ltd.  [jms]           Revised: 2004 Feb 11' */


#include <stdio.h>
#include <string.h>

#ifdef __AIX__
    #define RENAME  "mv"
#else
    #define RENAME  "ren"
#endif

#define MAXREPLACE  40
#define MAXNAME     260

static int  old_pos[MAXREPLACE];
static int  new_pos[MAXREPLACE];
static char template[MAXNAME];


int main( int argc, char *argv[] );
static int  setUp( const char *fname, const char *tname );
static int count( const char *str, char ch );
static void setPositions( const char *str, char ch, int *pos, int size );
static void toNew( void );
static void makeName( const char *src, char *dst );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nCreate rename script\n" );
    if (argc > 2)
        {
        if (!setUp( argv[1], argv[2] ))
            return 2;
        toNew( );
        return 0;
        }
    fprintf( stderr, "\nNeed from and to names with wild cards [question mark]\n" );
    return 1;
    }                                   /* main */


/* SETUP ===== Prepare global variables ===== $pageif:6 */

static int setUp( const char *fname, const char *tname )
    {                                   /* setUp */
    int n1, n2;
    memset( template, '\0', sizeof( template ) );
    memset( old_pos, '\0', sizeof( old_pos ) );
    memset( new_pos, '\0', sizeof( new_pos ) );
    if (strlen( tname ) >= sizeof( template ))
        {
        fprintf( stderr, "\nTo name is too long\n" );
        return 0;
        }
    n1 = count( fname, '?' );
    if (n1 == 0)
        {
        fprintf( stderr, "\nFrom name has no wild cards [question mark]\n" );
        return 0;
        }
    if (n1 > MAXREPLACE)
        {
        fprintf( stderr, "\nFrom name has too many wild cards [question mark]\n" );
        return 0;
        }
    n2 = count( tname, '?' );
    if (n2 != n1)
        {
        fprintf( stderr, "\nNumber of wild cards [question mark] in from name must match to name\n" );
        return 0;
        }
    setPositions( fname, '?', old_pos, MAXREPLACE );
    setPositions( tname, '?', new_pos, MAXREPLACE );
    strcpy( template, tname );
    return 1;
    }                                   /* setUp */


/* COUNT ===== Return number of character occurrances in string ===== $pageif:6 */

static int count( const char *str, char ch )
    {                                   /* count */
    int num = 0;
    if (str != NULL)
        {
        while (*str != '\0')
            if (*str++ == ch)
                ++num;
        }
    return num;
    }                                   /* count */


/* SETPOSTIONS ===== Scan for given character and note positions ===== $pageif:6 */

static void setPositions( const char *str, char ch, int *pos, int size )
    {                                   /* setPositions */
    if ((str != NULL) && (pos != NULL))
        {
        int i, k = 0;
        int lgth = strlen( str );
        for (i = 0;  i < lgth;  ++i)
            {
            if (str[i] == ch)
                {
                pos[k] = i;
                if (++k >= size)
                    break;
                }
            }
        while (k < size)
            pos[k++] = -1;
        }
    }                                   /* setPositions */


/* TONEW ===== Body of program ===== $pageif:6 */

static void toNew( void )
    {                                   /* toNew */
    char buf1[1024], buf2[1024];
    memset( buf1, '\0', sizeof (buf1) );
    memset( buf2, '\0', sizeof (buf2) );
    strcpy( buf2, template );

    while (scanf( "%s", buf1 ) == 1)
        {
        if (*buf1 != '.')
            {
            makeName( buf1, buf2 );
            if (strcmp( buf2, buf1 ) != 0)
                printf( "%s %s %s\n", RENAME, buf1, buf2 );
            }
        }
    }                                   /* toNew */


/* MAKENAME ===== Create instance of new name ===== $pageif:6 */

static void makeName( const char *src, char *dst )
    {                                   /* makeName */
    if ((src != NULL) && (dst != NULL))
        {
        int i, ps, pd;
        for (i = 0;  i < MAXREPLACE;  ++i)
            {
            ps = old_pos[i];
            pd = new_pos[i];
            if ((ps < 0) || (pd < 0))
                break;
            dst[pd] = src[ps];
            }
        }
    }                                   /* makeName */
