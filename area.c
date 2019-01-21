 
/* -------------------------------------------------------------*/
/* AREA.C ===== 1992 September 30, by J. Schwartz ===== */
 
/* Given two arrays x[] and y[] containing coordinates of points in */
/* a plane, and integer n, the number of points, calculate the area of the */
/* enclosed polygon.  This assumes the last point joins back to the first. */
/* Note that in C, arrays start at index zero. */
/* Hence the points are (x[0],y[0]), (x[1], y[1]), ... (x[n-1], y[n-1]). */
  
#include <math.h>
  
float area( float x[], float y[], int n )
    {                                                   /* area */
    int i;
    float sum = 0.0;
    for ( i = 1;  i < n;  ++i )
        sum += (x[i] - x[i - 1]) * (y[i] + y[i - 1]);   /* add pieces */
    sum += (x[0] - x[n - 1]) * (y[0] + y[n - 1]);       /* close loop */
    return fabs( sum / 2.0 );                           /* half abs val */
    }                                                   /* area */
 
/* -------------------------------------------------------------*/

/* Here is a simple calling program to demonstrate how it works */
 
#include <stdio.h>
  
int main()
    {                                                   /* main */
    float x[] = { 3., 10., 10., 5., 5., 3. };
    float y[] = { 3., 3., 10., 10., 7., 7. };
    float a = area( x, y, 6 );
 
    printf( "The area is %f\n", a );
    return 0;
    }                                                   /* main */
 
