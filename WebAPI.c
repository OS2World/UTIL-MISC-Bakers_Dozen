/* $title:      'WebAPI.C ===== Start Web API jar file =====' */
/* $subtitle:   'Elipse Ltd.  (jms)     Revised: 2014 Mar 28' */


#include <stdlib.h>
#include <string.h>
#include <process.h>

#define javaPath    "C:\Program Files\Java\jdk1.7.0_51\bin\java.exe"
#define trace       "-DTrace=On"
#define jarFile     "APIserver.jar"
#define outFile     ">APIout.txt"
#define errFile     ">APIerr.txt"


int main( int argc, char *argv[] )
    {
    execlp( javaPath, trace, jarFile, outFile, errFile, NULL );
    return 0;
    }
