/* title:       'SUBST.C ===== Substitute single character =====' */
/* subtitle:    'Elipse Ltd.  [jms]         Revised: 1998 Nov 07' */


/* Filter stdin to stdout where the first character is replaced by the second */
/* where a    denotes an ACSII char */
/*       nnnd deontes a  decimal number */
/*       nnnh denotes a  hex number */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHA   0
#define DEC     1
#define HEX     2

int main( int argc, char *argv[] );
void subst( int from, int to );
int char_value( char *str );
int classify( char *str );


/* MAIN ===== Entry point for program ===== $pageif:6 */

int main( int argc, char *argv[] )
    {                                   /* main */
    fprintf( stderr, "\nSUBST ===== (C) 1998, Elipse Ltd.\n" );
    if ( argc > 2 )
        {
        int from = char_value( argv[1] );
        int to = char_value( argv[2] );
        if ( (from >= 0) && (from < 256) && (to >= 0) && (to < 256) )
            {
            if ( to == from )
                {
                fprintf( stderr, "\nMust substitute different character\n" );
                return 2;
                }
            else
                {
                subst( from, to );
                return 0;
                }
            }
        }
    fprintf( stderr, "\nNeed characters FROM and TO\n" );
    return 1;
    }                                   /* main */


/* SUBST ===== Body of program ===== $pageif:6 */

void subst( int from, int to )
    {                                   /* subst */
    int ch;
    while ( (ch = getchar( )) != EOF )
        {
        if ( ch == from )
            putchar( to );
        else
            putchar( ch );
        }
    }                                   /* subst */


/* CHAR_VALUE ===== Turn string into character ===== $pageif:6 */

int char_value( char *str )
    {                                   /* char_value */
    int ch = -1;
    switch ( classify( str ))
        {
    case ALPHA:
        ch = (int) *str;
        break;
    case DEC:
        ch = strtol( str, NULL, 10 );
        break;
    case HEX:
        ch = strtol( str, NULL, 16 );
        break;
    default:
        ch = -1;
        }
    return ch;
    }                                   /* char_value */


/* CLASSIFY ===== Determine argument type ===== $pageif:6 */

int classify( char *str )
    {                                   /* classify */
    int dec, hex, flag, i, lgth;
    dec = hex = 0;
    lgth = strlen( str );
    if ( lgth < 2 )
        return ALPHA;

    for ( i = 0;  i < lgth;  ++i )
        {
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
