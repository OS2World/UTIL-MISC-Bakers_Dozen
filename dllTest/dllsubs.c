/* $title:      'DLLSUBS.C ===== Try out DLLs in OS/2 =====' */
/* $subtitle:   'Elipse Ltd.  [jms]    Revised: 1995 May 06' */


#include <stdio.h>
#include <string.h>

void dll_init( void );
void dll_get( char *buf, int len );
void dll_put( const char *buf );

static char string[2048];


/* DLL_INIT ===== Initialization ===== $pageif:6 */

void dll_init( void )
    {                                    /* dll_init */
    memset( string, '\0', sizeof( string ) );
    }                                    /* dll_init */


/* DLL_PUT ===== Save string ===== $pageif:6 */

void dll_put( const char *buf )
    {                                   /* dll_put */
    strncpy( string, buf, sizeof( string ) );
    }                                   /* dll_put */


/* DLL_GET ===== Return string ===== $pageif:6 */

void dll_get( char *buf, int len )
    {                                   /* dll_get */
    strncpy( buf, string, len );
    }                                   /* dll_get */
