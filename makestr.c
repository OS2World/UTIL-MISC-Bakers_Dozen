/* title:       'MAKESTR.C ===== Convert decimal to ASCII =====' */
/* subtitle:    'Elipse Ltd.  [jms]        Revised: 1993 Jul 21' */


/* Given al list of arguments, output the string equivalent */
/* where @xxx denotes an ACSII string */
/*       nnnd deontes a decimal number */
/*       nnnh denotes a hex number */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHA   0
#define DEC     1
#define HEX     2
#define MAXARG  255

int main( int argc, char *argv[] );
int classify( char *str );

/* MAIN ===== Entry point for program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    int i, v;
    char buf[MAXARG + 1];
    memset( buf, '\0', sizeof (buf) );

    for ( i = 1;  i < argc;  ++i )
        {
        strcpy( buf, argv[i] );
        v = classify( buf );
        switch ( v)
            {
        case ALPHA:
            printf( "%s", buf );
            break;
        case DEC:
            v = strtol( buf, NULL, 10 );
            if ( (v >= 0) && (v <= 255) )
                {
                printf( "%c", v );
                break;
                }
        case HEX:
            v = strtol( buf, NULL, 16 );
            if ( (v >= 0) && (v <= 255) )
                {
                printf( "%c", v );
                break;
                }
        default:
            printf( "%s", buf );
            }
        }
    return 0;
    }                                   /* main */


/* CLASSIFY ===== Determine argument type ===== $pageif:6 */

int classify( char *str )
    {                                   /* classify */
    int dec, hex, flag, i;
    dec = hex = 0;

    if ( *str == '@' )
        {
        strcpy( str, &str[1] );
        return ALPHA;
        }
    for ( i = 0;  i < MAXARG;  ++i )
        {
        if ( str[i] == '\0' )
            break;
        if ( isdigit( str[i] ) )
            ++dec;
        if ( isxdigit( str[i] ) )
            ++hex;
        }
    flag = i - 1;
    if ( (dec == flag) && (toupper( str[flag] ) == 'D') )
        {
        str[flag] = '\0';
        return DEC;
        }
    if ( (hex == flag) && (toupper( str[flag] ) == 'H') )
        {
        str[flag] = '\0';
        return HEX;
        }
    if ( dec == i )
        return DEC;
    if ( hex == i )
        return HEX;
    return ALPHA;
    }                                   /* classify */
