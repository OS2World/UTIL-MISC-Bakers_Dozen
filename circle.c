/* Circle.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int main( int argc, char *argv[] )
    {                                   /* main */
    int n = 0;
    double r = 0;
    printf( "\nCircle\n" );
    if ( argc < 3 )
        {
        printf( "\nParameter: Number of sides in polygon, radius\n" );
        return 1;
        }
    n = atoi( argv[1] );
    r = atof( argv[2] );
    if (( n > 2 ) && (r > 0.0))
        {
        double pi = 3.14159265358979323846;
        double twopi = pi + pi;
        double inc = twopi / (long double) n;
        double ang = 0.0;
        while ( ang <= (twopi + 0.001) )
            {
            printf( "%16.9f  (%7.3f)   -->  %16.9f, %16.9f\n",
                ang, ang * 180.0 / pi, r * cos( ang ), r * sin( ang ) );
            ang += inc;
            }
        }
    return 0;
    }                                   /* main */
