/* $title:      'VIO2.C ===== VIO Video test program =====' */
/* $subtitle:   'Elipse Ltd.  [jms]   Revised: 1997 Jan 09' */


#define INCL_VIO
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>

#define ATTRIB  0x07

static HVIO hvio;
static int cols = 80;
static int lines = 25;

int main( int argc, const char *argv[] );
int vio_test( int at );
int get_mode( void );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    int at = ATTRIB;
    fprintf( stderr, "\nVIO 1: Video test program\n" );
    if (argc > 1)
        sscanf( argv[1], "%x", &at );
    printf( "\n at = %x\n", at );
    get_mode( );

    _getch( );

    vio_test( at );
    return 0;
    }                                   /* main */


/* VIO_TEST ===== Body of program ===== $pageif:6 */

int vio_test( int at )
    {                                   /* vio_test */
    int i, j, rc, i1, i2;
    char str[4] = { 0, 0, 0, 0 };
    str[0] = ' ';
    str[1] = at;

    rc = VioWrtNCell( str, cols * lines, 0, 0, hvio );
    _getch( );

    for (i = 0;  i < lines;  ++i)
        {
        str[0] = '0' + i % 10;
        for (j = 0;  j < cols;  ++j)
            {
            rc = VioWrtNCell( str, 1, i, j, hvio );
            }
        }

    i1 = 2;
    i2 = lines - 2;
    for (i = 0;  i < 5;  ++i)
        {
        _getch( );
        rc = VioScrollUp( i1, 0, i2, cols, 1, ".\x70", hvio );
        }
    for (i = 0;  i < 5;  ++i)
        {
        _getch( );
        rc = VioScrollDn( i1, 0, i2, cols, 1, ".\x70", hvio );
        }
    _getch( );
    rc = VioWrtNCell( " \x07", cols * lines, 0, 0, hvio );

    return 0;
    }                                   /* vio_test */


/* GET_MODE ===== Set globals for lines and columns ===== $pageif:6 */

int get_mode( void )
    {                                   /* get_mode */
    int rc;
    VIOMODEINFO mdata;
    memset( &mdata, '\0', sizeof( mdata ) );
    mdata.cb = sizeof( mdata );
    rc = VioGetMode( &mdata, hvio );
    cols = mdata.col;
    lines = mdata.row;

    printf( "\nReturn from VioGetMode = %d\n", rc );
    printf( "Bitmask: %d\n", (int) mdata.fbType );
    printf( "Colours: %d\n", (int) mdata.color );
    printf( "Columns: %d\n", (int) mdata.col );
    printf( "Rows:    %d\n", (int) mdata.row );
    printf( "H-res:   %d\n", (int) mdata.hres );
    printf( "V-res:   %d\n", (int) mdata.vres );
    printf( "NumAtt:  %d\n", (int) mdata.attrib );
    printf( "AttFmt:  %d\n", (int) mdata.fmt_ID );
    printf( "BufLen:  %d\n", (int) mdata.buf_length );
    return 0;
    }                                   /* get_mode */
