/* $title:       'CRC.C ===== print CRC of given file =====' */
/* $subtitle:   'Elipse Ltd.  (jms)   Revised: 1997 Oct 07' */

/* Code originally from EDM/2 e-zine Vol. 5, No. 10 */

#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] );

unsigned long crc_get_crc( FILE *fp, unsigned long crctable[] );
void crc_gen_table( unsigned long crctable[] );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, char *argv[] )
    {
    unsigned long crctable[256];
    int i;
    fprintf( stderr, "\n\n CRC ===== Generate CRC for given file(s).  %s\n\n", __DATE__ );

    if (argc < 2)                       /* Check number of arguments */
        {
        fprintf( stderr, "\nMust name at least one file\n" );
        return 1;
        }

    crc_gen_table( crctable );          /* Generate the table */

    for (i = 1;  i < argc;  ++i)
        {
        FILE *fp = fopen( argv[i], "rb" );
        if (fp != NULL)
            {
            unsigned long crc = crc_get_crc( fp, crctable );
            printf( " %08lX  %s\n", crc, strlwr( argv[i] ) );
            fclose( fp );
            }
        }

    printf( " \n" );
    return 0;
    }


/* CRC_GET_CRC ===== Returns CRC value of an open file ===== $pageif:6 */

unsigned long crc_get_crc( FILE *fp, unsigned long crctable[] )
    {
    unsigned long crc = 0xFFFFFFFF;
    int c;

    while ((c = getc( fp )) != EOF) 
        crc = ((crc >> 8) & 0x00FFFFFF) ^ crctable[(crc ^ c) & 0xFF];

    crc ^= 0xFFFFFFFF;
    return crc;
    }


/* CRC_GEN_TABLE ===== Generate table of values used in crc_get_crc ===== $pageif:6 */

void crc_gen_table( unsigned long crctable[] )
    {
    unsigned long crc, poly;
    int i, j;

    poly = 0xEDB88320L;
    for (i = 0;  i < 256;  i++) 
        {
        crc = i;
        for ( j = 8;  j > 0;  j--) 
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else 
                crc >>= 1;
        crctable[i] = crc;
        }
    }
