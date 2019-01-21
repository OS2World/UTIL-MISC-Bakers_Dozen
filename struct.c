/* $title:      'STRUCT.C ==== Make C structures for files =====' */
/* $subtitle:   'Elipse Ltd.  [jms]         Revised: 2001 Jun 03' */


#include <stdio.h>
#include <string.h>
#include <bfile.h>
#include <screen.h>

#ifdef InstantC
extern __structAlignment = 1;
#endif
#ifdef __ZTC__
#pragma ZTC align 1
#endif
#ifdef __IBMC__
#pragma pack(1)
#endif

static int hfil = -1;
static int hfld = -1;
static FILE *tfile = NULL;

typedef unsigned short ushort;

typedef struct
    {
    short Xf_Id;
    char  Xf_Name[20];
    char  Xf_Loc[64];
    char  Xf_Flags;
    char  Xf_Pad[10];
    } RFIL;

typedef struct
    {
    short  Xfe_Id;
    short  Xfe_File;
    char   Xfe_Name[20];
    char   Xfe_DataType;
    short  Xfe_Offset;
    ushort Xfe_Size;
    char   Xfe_Dec;
    short  Xfe_Flags;
    } RFLD;

int main( int argc, const char *argv[] );
static int structure( const char *filename );
static void heading( const char *name );
static void footing( void );
static void file_loop( void );
static void field_loop( int fileid );
static void field_output( RFLD *fptr );
static char *trimsp( const char *str, unsigned maxlen );
static char *make_lendef( const char *str, unsigned lgth );


/* MAIN ===== Starting point ===== $pageif:6 */

int main( int argc, const char *argv[] )
    {                                   /* main */
    char filename[24];
    int ret;
    fprintf( stderr, "\nSTRUCT ===== Create C structures for Btrieve files =====\n" );
    fprintf( stderr, "\n             (C) Elipse Limited, %s\n\n", __DATE__ );
    memset( filename, '\0', sizeof (filename) );
    init_bfiles( 1 );
    if ( argc > 1 )
        strncpy( filename, argv[1], sizeof( filename ) );
    else
        strcpy( filename, "STRUCT" );
    strupr( filename );
    ret = structure( filename );
    b_closeall( );
    if ( ret > 0 )
        fprintf( stderr, "\nProgram failed.  (error %d)\n", ret );
    return ret;
    }                                   /* main */


/* STRUCTURE ===== Body of program ===== $pageif:6 */

static int structure( const char *filename )
    {                                   /* structure */
    int ch = 0;
    hfil = b_open( "file.ddf", sizeof(RFIL), B_OPEN_RO );
    if ( hfil < 0 )
        {
        fprintf( stderr, "\nUnable to open file.ddf!\n" );
        return 1;
        }
    set_keyno( hfil, 1, B_NOLOCK );     /* Sort by file name */
    hfld = b_open( "field.ddf", sizeof(RFLD), B_OPEN_RO );
    if ( hfld < 0 )
        {
        fprintf( stderr, "\nUnable to open field.ddf!\n" );
        return 2;
        }
    set_keyno( hfld, 1, B_NOLOCK );     /* Sort by file number */
    tfile = tmpfile( );
    heading( filename );
    if (tfile == NULL)
        tfile = stdout;
    printf( "\n" );
    file_loop( );
    printf( "\n" );
    rewind( tfile );
    while ((ch = fgetc( tfile )) != EOF)
        putchar( ch );
    fclose( tfile );
    footing( );
    return 0;                           /* all done */
    }                                   /* structure */


/* HEADING ===== Output title lines of structure file ===== $pageif:6 */

static void heading( const char *filename )
    {                                   /* heading */
    printf( "/* $title:     '%s.H ===== Btrieve File Structures =====' */\n", filename );
    printf( "/* $subtitle:  'Elipse Ltd.  [jms]      " );
    printf( "Revised: %s' */\n\n\n", format_adate( current_date( ) ) );
    printf( "#ifndef %s_H\n", filename );
    printf( "#define %s_H 1\n", filename );
    printf( "\n/* ----- Begin File Structures ----- */\n\n" );
    printf( "\n#include <btrieve.h>\n\n" );
    }                                   /* heading */


/* FOOTING ===== Output final lines of structure file ===== $pageif:6 */

static void footing( void )
    {                                   /* footing */
    printf( "\n/* ----- End File Structures ----- */\n\n" );
    printf( "#endif\n\n" );
    }                                   /* footing */


/* FILE_LOOP ===== Scan each file ===== $pageif:6 */

static void file_loop( void )
    {                                   /* file_loop */
    RFIL filerec;
    char *sname;
    if ( search_first( hfil, &filerec ) == 0 )
        {
        do  
            {
            if ( (filerec.Xf_Flags & 0x10) == 0 )
                {
                fprintf( tfile, "typedef struct\n\t{\n" );
                field_loop( filerec.Xf_Id );
                sname = trimsp( filerec.Xf_Name, sizeof (filerec.Xf_Name) );
                fprintf( tfile, "\t} %s;\n\n", strupr( sname ) );
                }
            }
        while ( search_next( hfil, &filerec ) == 0 );
        }
    }                                   /* file_loop */


/* FIELD_LOOP ===== Output fields for given file ===== $pageif:6 */

static void field_loop( int fileid )
    {                                   /* field_loop */
    RFLD fldrec;
    int key = fileid;

    if ( search_from( hfld, &fldrec, &key ) == 0 )
        {
        do  
            {
            if ( fldrec.Xfe_File > fileid )
                break;
            field_output( &fldrec );
            }
        while ( search_next( hfld, &fldrec ) == 0 );
        }
    }                                   /* field_loop */


/* FIELD_OUTPUT ===== Output info for one field ===== $pageif:6 */

static void field_output( RFLD *fptr )
    {                                   /* field_output */
    char *lendef;
    unsigned short numwords = 0;
    char *fname = trimsp( fptr->Xfe_Name, sizeof (fptr->Xfe_Name) );
    switch ( fptr->Xfe_DataType )
        {
    case 0:                             /* String */
        fprintf( tfile, "\tchar %s[%d];\t\t/* String - neither length nor nul */\n", fname, fptr->Xfe_Size );
        break;
    case 1:                             /* Int */
        switch ( fptr->Xfe_Size )
            {
        case 1:
            fprintf( tfile, "\tchar %s;\n", fname );
            break;
        case 2:
            fprintf( tfile, "\tshort %s;\n", fname );
            break;
        case 4:
            fprintf( tfile, "\tlong %s;\n", fname );
            break;
            }
        break;
    case 2:                             /* Float */
        switch ( fptr->Xfe_Size )
            {
        case 4:
            fprintf( tfile, "\tfloat %s;\n", fname );
            break;
        case 8:
            fprintf( tfile, "\tdouble %s;\n", fname );
            break;
            }
        break;
    case 3:                             /* Date */
        fprintf( tfile, "\tb_date %s;\n", fname );
        break;
    case 4:                             /* Time */
        fprintf( tfile, "\tb_time %s;\n", fname );
        break;
    case 5:                             /* Decimal */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* Packed decimal */\n", fname, lendef );
        break;
    case 6:                             /* Money */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* Packed dec (Money) */\n", fname, lendef );
        break;
    case 7:                             /* Logical */
        switch ( fptr->Xfe_Size )
            {
        case 1:
            fprintf( tfile, "\tchar %s;   \t\t/* Logical */\n", fname );
            break;
        case 2:
            fprintf( tfile, "\tchar %s[2];\t\t/* Logical */\n", fname );
            break;
            }
        break;
    case 8:                             /* Numeric */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* ASCII number */\n", fname, lendef );
        break;
    case 9:                             /* Bfloat */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* BASIC float */\n", fname, lendef );
        break;
    case 10:                            /* Lstring */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* First byte = length */\n", fname, lendef );
        break;
    case 11:                            /* Zstring */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\n", fname, lendef );
        break;
    case 12:                            /* Note */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* Note - Variable length */\n", fname, lendef );
        break;
    case 13:                            /* Lvar */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* Lvar - Variable length */\n", fname, lendef );
        break;
    case 14:                            /* Unsigned binary */
        numwords = (ushort) (fptr->Xfe_Size) >> 1;
        if ( numwords < (ushort) 2 )
            fprintf( tfile, "\tunsigned short %s;\n", fname );
        else
            fprintf( tfile, "\tunsigned short %s[%d];\n", fname, numwords );
        break;
    case 15:                            /* Autoincrement */
        switch ( fptr->Xfe_Size )
            {
        case 2:
            fprintf( tfile, "\tunsigned short %s;\t\t/* AutoIncrement */\n", fname );
            break;
        case 4:
            fprintf( tfile, "\tunsigned long %s;\t\t/* AutoIncrement */\n", fname );
            break;
            }
        break;
    case 16:                            /* Bit */
        fprintf( tfile, "\tunsigned %s : 1;\n", fname );
        break;
    default:                            /* Unknown type */
        lendef = make_lendef( fname, fptr->Xfe_Size );
        fprintf( tfile, "\tchar %s[%s];\t\t/* Unknown data type */\n", fname, lendef );
        }
    }                                   /* field_output */


/* TRIMSP ===== Trim spaces on right ===== $pageif:6 */

static char *trimsp( const char *str, unsigned maxlen )
    {                                   /* trimsp */
    static char result[44];
    memset( result, '\0', sizeof (result) );
    if ((str != NULL) && (maxlen > 0))
        {
        unsigned i = maxlen > 40 ? 40 : maxlen;
        strncpy( result, str, maxlen );
        while ( --i > 1 )
            {
            if ( result[i] > ' ' )
                break;
            result[i] = '\0';
            }
        }
    return result;
    }                                   /* trimsp */


/* MAKELENDEF ===== Create defined variable for string length ===== $pageif:6 */

static char *make_lendef( const char *str, unsigned lgth )
    {                                   /* make_lendef */
    static char lendef[52];
    memset( lendef, '\0', sizeof (lendef) );
    if ((str != NULL) && (lgth > 0))
        {
        strncpy( lendef, "MAX_", sizeof (lendef) );
        strncat( lendef, str, sizeof (lendef) );
        strupr( lendef );
        printf( "#define %-19s %d\n", lendef, lgth );
        }
    return lendef;
    }                                   /* make_lendef */
