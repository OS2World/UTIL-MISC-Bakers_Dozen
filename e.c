/* e.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void e( long a, long b );


int main( int argc, const char *argv[] )
    {
    long a, b;
    fprintf( stderr, "\nApproximate e\n" );
    if ( argc < 3 )
        {
        fprintf( stderr, "\nMust give start and stop values\n" );
        return 1;
        }
    a = atol( argv[1] );
    b = atol( argv[2] );
    if ( (a < 1) || (b < a) )
        {
        fprintf( stderr,
            "\nBoth numbers must be positive and the second greater\n" );
        return 2;
        }
    e( a, b );
    return 0;
    }


void e( long a, long b )
    {
    long i;
    for ( i = a;  i <= b;  ++i )
        {
        double n = (double) i;
        double y = pow( (1.0 + 1.0 / n), n );
        printf( "%9ld %20.15lf \n", i, y );
        }
    }
