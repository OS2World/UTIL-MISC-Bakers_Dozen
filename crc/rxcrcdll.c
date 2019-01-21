/* RXCRCDLL.C */

#include <stdio.h>
#include <string.h>


/* This is the exported function, the one called from REXX */

ULONG RxGetCrc( CHAR *name,         /* Routine name */
                ULONG numargs,      /* Number of arguments */
                RXSTRING args[],    /* Null-terminated RXSTRINGs array*/
                CHAR *queuename,    /* Current external data queue name */
                RXSTRING *retstr)   /* returning RXSTRING  */
    {
    char string[9];
    unsigned long crc;
    FILE *fp;
    unsigned long crcTable[256];

    /* Check number of arguments */
    if (numargs != 1)
        return RETSTR_INVALID; 

    /* Generate the table */
    CrcGenTable( crcTable );

    /* The output string will be ERROR or the CRC value */
    if ((fp = fopen( args[0].strptr, "rb" ))==NULL)
        {
        strcpy( string, "ERROR" );
        }
    else 
        {
        crc = CrcGetCrc( fp, crcTable );
        sprintf( string, "%08lx", crc);
        }

    fclose( fp );

    /* Copy the output string to the returned REXX string */
    strcpy(retstr->strptr,string);
    retstr->strlength = strlen(string);
    
    /* Success !!! */
    return RETSTR_OK;
    }


/* This function returns the CRC value of an opened file */

unsigned long CrcGetCrc( FILE *fp,unsigned long* crcTable )
    {
    unsigned long crc = 0xFFFFFFFF;
    int c;

    while ((c = getc( fp )) != EOF) 
        crc = ((crc >> 8) & 0x00FFFFFF) ^ crcTable[(crc ^ c) & 0xFF];

    crc ^= 0xFFFFFFFF;
    return crc;
    }


/* This function generates a table of values used in CrcGetCrc */

void CrcGenTable( unsigned long* crcTable )
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
        crcTable[i] = crc;
        }
    }
