/* title:    'CIPHER.C ===== Encipher or Decipher files =====' */
/* subtitle: 'Elipse Ltd.  (jms)         Revised: 2002 Mar 22' */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <sys\types.h>
#include <sys\stat.h>

/* internal function prototypes */

static int param_routine( int, char *, char *, char *, char *, char *); /* determine parameters */
static void eval_task( char * );                                        /* evaluate parameter for task */
static void iprintf( int );                                             /* print input prompts */
static char *cp_fgets( char *, int );                                   /* get parameters from stdin */
static int ciphelp( void );                                             /* list help text */
static int c_d( void );                                                 /* cipher or decipher */
static int build_funct_parms( void );                                   /* sets funct parms from key */
static int build_cipher( void );                                        /* build cipher mapping */
static int functn( int );                                               /* bits to skip for build_cipher */
static unsigned int read_input( unsigned int );                         /* read input stream */
static unsigned int write_out( unsigned int );                          /* write output stream */
static int transpose( void );                                           /* masks and remaps bits */
static int open_in_file( void );                                        /* open input file */
static int open_out_file( void );                                       /* open output file */
static int open_out_file_exists( void );                                /* open existing output file */
static void close_in_file( void );                                      /* close input file */
static void close_out_file( void );                                     /* close output file */
static void usage( int );                                               /* error handling routine */


static char *cp_infile, *cp_outfile, *cp_keybuf, *cp_overwrt, *cp_task; /* pointers to input parameter variables */

static int idigit;                                                      /* number of digits in key */

static char ca_help_msg[17][75] =
    {                                   /* help listing strings - allows for later reading from file */
    "               ABC1  - A Bit Cypher (Version 1)\n\n",
    "               ABC1 ciphers and deciphers files.\n\n",
    "  If ABC1 is run without parameters, it will prompt for them.\n\n",
    "      ABC1 <Input File> <Output File> <Key> <task> <Overwrite> \n\n",
    "         Task:       -E(S) Encipher, -D(T) Decipher, -H Help\n",
    "         Input File:   the file to be either Enciphered or Deciphered.\n",
    "         Output file:  the resulting file from Encypering or Deciphering.\n",
    "         Key:          a number from 0 - 9999999999 (up to 10 digits).\n",
    "            This number is used to generate a unique ciphering of data.\n",
    "            When the same number is used to decipher the ciphered data,\n",
    "            the orignal data is reproduced.\n",
    "         Overwrite:   when 'Y', the output file may be overwritten,\n",
    "            otherwise when the output file name exists ABC1 will stop.\n\n",
    "  Examples: \n",
    "    ABC1 test.dat test.enc 343228 -e -o\n",
    "    ABC1 test.enc test.new 343228 -d -n\n"
    };

static char ca_err_msg[25][60] =
    {                                   /* error message strings */
    " Error Routine, usage( ), With Zero Argument. \n",
    " Error in Extracting Parameters. \n",
    " Error in Help Procedure. \n",
    " Error in Cypher/Decipher Procedure. \n",
    " Input File Does NOT Exist. \n",
    " Input File Open Failed. \n",
    " Output file exists but No Overwrite Option. \n",
    " Output file open failed. \n",
    " Input key may only be 10 numerics long.\n",
    " Unexpected Digit in Key Value. \n",
    " Error in Building Function from Key. \n",
    " Error in Building Cypher Mapping. \n",
    " Read of Input File Failed. \n",
    " Write of Output File Failed. \n",
    " Transposition Algorithm Failed. \n",
    " Key Not Passed as Parameter. \n",
    " Close of Input File Failed. \n",
    " Close of Output File Failed. \n",
    " Error Printing Prompts for Input. \n",
    " Error Accepting Input. \n",
    " Error In Processing Input. \n",
    " System Error Message",
    " 22 "
    };

enum Err_Handle 
    {                                   /* enumerated references for Error message strings */
    EROUTINEZ,                          /* 0 */
    EREXTPARM,
    ERHELP,
    ERCD,
    ERIFILENOT,
    ERIFILEOPEN,                        /* 5 */
    EROFILENO,
    EROFILEOPEN,
    ERKEY10,
    ERNOTNUM,
    ERBUILDF,                           /* 10 */
    ERBUILDCIPH,
    ERREADIN,
    ERWRITEOUT,
    ERTRANSPOSE,
    ERABCXNOKEY,                        /* 15 */
    ERIFILECLOS,
    EROFILECLOS,
    ERPRINT,
    ERSTDIN,
    ERINPUTCR,
    ERPERROR,
    } err_handle;

static char ca_inp_msg[8][60] =
    {                                   /* input prompt strings */
    " Zero Passed to Print Routine ",
    " Enter E (Encipher), D (Decipher), or H (Help) : ",
    " Enter input file name : ",
    " Enter output file name : ",
    " Output File Exists. Overwrite? (O or N) : ",
    " Enter key, 0-9999999999 : ",
    " 7 "
    };

enum Inp_Handle 
    {                                   /* enumerated reference for input prompt strings */
    INPROUTINEZ,                        /* 0 */
    INPEDH,
    ININFILENM,
    INOUTFILENM,
    INOUTFILEYN,
    INKEY,                              /* 5 */
    IN7
    } inp_handle;

static enum Task 
    {                                   /* enumerate the options to be performed */
    DEFAULT,
    ENCIPH,
    DECIPH,
    HELP
    } task;

/* type definitions to assure compatable types used in bit-wise logic */

typedef unsigned long  int four_byte;   /* this is to be 32 bits long */
typedef unsigned short int two_byte;    /* this is to be 16 bits long */

/* input/output stream variables */

#define iotwobytes 512                  /* size, in two bytes, of io reads & writes */

static two_byte tba_ibuffer[iotwobytes];    /* input stream array */
static two_byte tba_obuffer[iotwobytes];    /* output stream array */
static int iinfile = -1;                    /* input file handle */
static int ioutfile = -1;                   /* output file handle */
static unsigned short *puibuf = NULL;       /* pointer to input stream array */
static unsigned short *puobuf = NULL;       /* pointer to output stream array */


static two_byte tb_mask = 0;                /* mask exclusive ORed (EORed) to input before encipher transpose,
                                               to output after decipher transpose. */

#define cipherbits 8192                     /* number of bits mapped in transposition */

#define iobytes 1024                        /* number of bytes in input & output file buffers */


static char ca_done[cipherbits];            /* to track bit positions already assigned
                                               (set to 1 when position in ia_new_pos is assigned) */

static int ia_new_pos[cipherbits];          /* ith entry holds mapped to position for ith bit for enciphering,
                                               mapped from position for deciphering */


static int ia_inc_b2[7] = {0, 0, 0, 0, 0, 0, 0};    /* used in funct, determined from key (ul_key),
                                                       in funct(x), the ith entry is added if modulus 2**i of x is zero */

/* working variables */

static int saverrno;
static two_byte tb_work;                /* two byte working variable */
static four_byte fb_work, fb_worka;     /* four byte working variables */
static four_byte fb_key;                /* key - after decrease down to four bytes */
static int i_bit_pos = 0;               /* index for bit position in transposing */

main( int argc, char **argv )
    {
    /* determine parameters from arguments or input */
    if (param_routine( argc, argv[1], argv[2], argv[3], argv[4], argv[5] ) == EXIT_FAILURE) 
        {
        usage( EREXTPARM );
        return EXIT_FAILURE;
        }
    if (task == HELP)
        {
        if (ciphelp( ) == EXIT_FAILURE) 
            {
            usage( ERHELP );
            return EXIT_FAILURE;
            }
        return EXIT_SUCCESS;
        }
    if (task == ENCIPH || task == DECIPH)
        {                               /* cipher/decipher input stream file */
        if (c_d( ) == EXIT_FAILURE) 
            {
            usage( ERCD );
            return EXIT_FAILURE;
            }
        close_in_file( );
        close_out_file( );
        }
    return EXIT_SUCCESS;
    }


/* functn( ) is */
/* called by build cipher, which passes an integer from 0 - cipherbits    */
/* this function is designed to be unique for each key 0 - 10,000,000,000 */
/* based on ia_inc_b2[] entries being added 1, 4, 8, 16, 32, 64, 128      */
/* (see build functn_parms)                                               */
/* and these being added to the returned result the input (pos_new)       */
/* being a multiple (mod == 0) of 4, 8, 16, 32, 64, 128 for positions     */
/* 1, 2, 3, 4, 5, 6 respectively of ia_inc_b2[]                           */


int functn( int pos_new )
    {
    int i_acc;          /* accumulate what is to be returned */
    int ix_b2;          /* index for ia_inc_b2[] */
    int inc_bin;

    /* is 4 * (2 ** ix_inc_b2), used e.g. to have the 2nd entry */
    /* add every fourth time, 3rd every 8th, 4th every 16th ... */

    i_acc = ia_inc_b2[0] + 2;
    for (inc_bin = 4, ix_b2 = 1;
            ix_b2 < 7;
            inc_bin = inc_bin * 2, ix_b2++) 
        {
        if ((pos_new % inc_bin) == 0) 
            i_acc = i_acc + ia_inc_b2[ix_b2];
        }
    /* return modulus of remaining positions, for efficiency (same result) */
    i_acc = i_acc % (cipherbits - pos_new);
    return i_acc;
    }


int build_cipher( void )
    {                                   /* build bit mapping array array */
    int pos_new;                        /* values 0 to cipherbits (8124) */
    int hops;
    int pos_old = 0;

    /* initialize to "no positions done", i.e. all zeros */

    for (pos_new = 0; pos_new < cipherbits; pos_new++)
        ca_done[pos_new] = 0;

    /* for each new position (position in ia_new_pos),
       determine the old position mapped to it */

    for (pos_new = 0; pos_new < cipherbits; pos_new++) 
        {
        hops = functn( pos_new );
        /* shouldn't happen with functn( ) as defined here */
        /* but allow for changes to functn( ) */
        if (hops == 0) 
            hops = 3;
        while (hops != 0) 
            {
            if (pos_old == (cipherbits - 1)) 
                pos_old = 0;
            else 
                pos_old++;
            if (ca_done[pos_old] == 0) 
                {
                if (hops > 0) 
                    hops--;
                else 
                    hops++;
                }
            }
        ca_done[pos_old] = 1;
        ia_new_pos[pos_new] = pos_old;
        }
    return EXIT_SUCCESS;
    }


/* transpose the input stream buffer to the output stream buffer,
   using the mask and bit mapping (cipher) */

int transpose( void )
    {
    div_t divres;
    int word_pos = 0;       /* values 0 to iotwobytes (512) */
    int out_pos, bpos_orig, in_pos; /* values 0 to iotwobytes (512) */
    int bper_word = 16;     /* bits per two bytes */

    if (task == ENCIPH) 
        {                   /* first mask over the input stream for enciphering */
        for (in_pos = 0; in_pos < iotwobytes; in_pos++) 
            tba_ibuffer[in_pos] = tba_ibuffer[in_pos] ^ tb_mask;
        }
    /* set the output buffer to zeros */
    for (out_pos = 0; out_pos < iotwobytes; out_pos++) 
        tba_obuffer[out_pos] = 0;

    /* for all bit positions in  input buffer ( bpos_orig ),
    and cycle through each bit ( i_bit_pos )
    of each 'word' ( word_pos )
    set the output bit in the mapped position if it to be "1" */
    for (bpos_orig = 0, i_bit_pos = 0; bpos_orig < cipherbits; bpos_orig++) 
        {
        if (task == ENCIPH) 
            {
            tb_work = 1;
            tb_work = tb_work << i_bit_pos;
            /* set output bit only if "1" bit input i_bit_pos */
            if (tba_ibuffer[word_pos] & tb_work) 
                {
                divres = div( ia_new_pos[bpos_orig], bper_word );
                tb_work = 1;
                tb_work = tb_work << divres.rem;
                /* bitwise exclusive or ^ */
                tba_obuffer[divres.quot] = tba_obuffer[divres.quot] ^ tb_work;
                }
            } 
        else 
            {                           /* task == DECIPH */
            divres = div( ia_new_pos[bpos_orig], bper_word );
            tb_work = 1;
            tb_work = tb_work << divres.rem;
            if (tba_ibuffer[divres.quot] & tb_work) 
                {
                tb_work = 1;
                tb_work = tb_work << i_bit_pos;
                tba_obuffer[word_pos] = tba_obuffer[word_pos] ^ tb_work;
                }
            }
        i_bit_pos++;
        if (i_bit_pos == 16) 
            {
            word_pos++;
            i_bit_pos = 0;
            }
        }
    if (task == DECIPH) 
        {                               /* last unmask over output stream for deciphering */
        for (out_pos = 0; out_pos < iotwobytes; out_pos++) 
            tba_obuffer[out_pos] = tba_obuffer[out_pos] ^ tb_mask;
        }
    return EXIT_SUCCESS;
    }


/* take in and evaluate input parameters for ABC1 */

int param_routine( int iparm, char *cein, char *ceout, char *ceseed, char *ceopt, char *ceoverwrt )
    {

    /* !! need to tidy up "-", "/" constants and option constants */
    /* and STD stream constants and parameters in various order   */

    static char ca_infbuf[1025];    /* input file name */
    static char ca_outfbuf[1025];   /* output file name */
    static char ca_keybuf[1025];    /* cipher key number */
    static char ca_optbuf[1025];    /* cipher task - e.g. "-D", "-E" */
    static char ca_overwrtbuf[1025];    /* overwrite option - "O" or "N" */

    /* setting pointers to buffers */

    cp_infile = ca_infbuf;
    cp_outfile = ca_outfbuf;
    cp_keybuf = ca_keybuf;
    cp_overwrt = ca_overwrtbuf;
    cp_task = ca_optbuf;

    if (iparm > 2) 
        {
        if ((strcmp( ceout, "/S" ) == 0)
                || (strcmp( ceout, "/s" ) == 0)
                || (strcmp( ceout, "/T" ) == 0)
                || (strcmp( ceout, "/t" ) == 0)
                || (strcmp( ceout, "-S" ) == 0)
                || (strcmp( ceout, "-s" ) == 0)
                || (strcmp( ceout, "-T" ) == 0)
                || (strcmp( ceout, "-t" ) == 0)) 
            {
            cp_keybuf = strdup( cein );
            for (idigit = 0; idigit < 12; idigit++) 
                {
                if (!isdigit( cp_keybuf[idigit] ))
                    break;
                }
            if (idigit > 10) 
                {
                usage( ERKEY10 );
                return EXIT_FAILURE;
                }
            if ((strcmp( ceout, "-S" ) == 0)
                    || (strcmp( ceout, "-s" ) == 0)
                    || (strcmp( ceout, "/S" ) == 0)
                    || (strcmp( ceout, "/s" ) == 0)) 
                task = DECIPH;
            else 
                task = ENCIPH;
            return EXIT_SUCCESS;
            }
        }
    if (iparm > 4) 
        {                               /* ceopt is not NULL */
        eval_task( ceopt );
        }
    if ((task != ENCIPH) && (task != DECIPH) && (task != HELP)) 
        {
        while ((task != ENCIPH) && (task != DECIPH) && (task != HELP)) 
            {
            /* receive task option from stdin */
            iprintf( INPEDH );
            cp_task = cp_fgets( ca_optbuf, 3 );
            eval_task( cp_task );
            }
        }
    if (task == HELP) 
        return EXIT_SUCCESS;
    if (iparm > 1) 
        cp_infile = strdup( cein );
    else 
        {                               /* receive input filename from stdin */
        iprintf( ININFILENM );
        cp_infile = cp_fgets( ca_infbuf, 1026 );
        }
    if (iparm > 2) 
        cp_outfile = strdup( ceout );
    else 
        {                               /* receive output filename from stdin */
        iprintf( INOUTFILENM );
        cp_outfile = cp_fgets( ca_outfbuf, 1026 );
        }
    /* Open input file and create output file, overwriting if necessary. */
    if (open_in_file( ) == -1) 
        {
        saverrno = errno;
        if (saverrno == 2) 
            usage( ERIFILENOT );
        else 
            {
            usage( ERIFILEOPEN );
            return EXIT_FAILURE;
            }
        } 
    if (open_out_file( ) == -1) 
        {
        saverrno = errno;
        if (saverrno == EEXIST) 
            {
            if (iparm > 5) 
                cp_overwrt = strdup( ceoverwrt );
            else 
                {                       /* check if output file to be overwritten */
                iprintf( INOUTFILEYN );
                cp_overwrt = cp_fgets( ceoverwrt, 3 );
                }
            if ((strcmp( cp_overwrt, "-O" ) == 0)
                    || (strcmp( cp_overwrt, "-o" ) == 0)
                    || (strcmp( cp_overwrt, "/O" ) == 0)
                    || (strcmp( cp_overwrt, "/o" ) == 0)
                    || (strcmp( cp_overwrt, "O" ) == 0)
                    || (strcmp( cp_overwrt, "o" ) == 0)) 
                {
                if (open_out_file_exists( ) == -1) 
                    {
                    usage( EROFILENO );
                    return EXIT_FAILURE;
                    } 
                else 
                    errno = 0;          /* prevent wrong error message if later error */
                }
            } 
        else 
            {
            usage( EROFILENO );
            return EXIT_FAILURE;
            }
        } 
    if (ioutfile == -1) 
        {
        usage( EROFILEOPEN );
        return EXIT_FAILURE;
        }
    if (iparm > 4) 
        cp_keybuf = strdup( ceseed );
    else 
        {                               /* receive key from stdin */
        iprintf( INKEY );
        cp_keybuf = cp_fgets( ca_keybuf, 12 );
        }
    for (idigit = 0; idigit < 12; idigit++) 
        {
        if (!isdigit( cp_keybuf[idigit] ))
            break;
        }
    if (idigit > 10) 
        {
        usage( ERKEY10 );
        return EXIT_FAILURE;
        }
    return EXIT_SUCCESS;
    }


int build_funct_parms( void )
    {
    /* !! Needs to be changed to create key bit array from digits */

    char *stopstring;                   /* points to char that stopped the scan */
    div_t div_res_bp;                   /* used to determine bit position  */
    int i_mask_pos = -1;                /* bit position in tb_mask */

    if (idigit == 10) 
        {
        fb_work = 1;
        fb_work = fb_work << 6;
        switch (cp_keybuf[0]) 
            {
        case '9':
            cp_keybuf[0] = '3';         /* set bit 36 */
            ia_inc_b2[5] = ia_inc_b2[5] ^ fb_work;
            break;
        case '8':
            cp_keybuf[0] = '2';         /* set bit 35 */
            ia_inc_b2[4] = ia_inc_b2[4] ^ fb_work;
            break;
        case '7':
            cp_keybuf[0] = '5';         /* set bit 34 */
            ia_inc_b2[3] = ia_inc_b2[3] ^ fb_work;
            break;
        case '6':
            cp_keybuf[0] = '1';         /* set bit 34 */
            ia_inc_b2[3] = ia_inc_b2[3] ^ fb_work;
            break;
        case '5':
            cp_keybuf[0] = '3';         /* set bit 33 */
            ia_inc_b2[2] = ia_inc_b2[2] ^ fb_work;
            break;
        case '4':
            cp_keybuf[0] = '2';         /* set bit 33 */
            ia_inc_b2[2] = ia_inc_b2[2] ^ fb_work;
            break;
        default:
            usage(ERNOTNUM);
            return EXIT_FAILURE;
            }
        }
    fb_key = strtoul( cp_keybuf, &stopstring, 10 );
    /* split seed */
    for (i_bit_pos = 0; i_bit_pos < 32; i_bit_pos++) 
        {
        fb_worka = 1;
        fb_worka = fb_worka << i_bit_pos;
        if (fb_worka & fb_key) 
            {
            div_res_bp = div(i_bit_pos, 6);
            fb_work = 1;
            fb_work = fb_work << div_res_bp.quot;
            ia_inc_b2[div_res_bp.rem] = ia_inc_b2[div_res_bp.rem] ^ fb_work;
            }
        /* build up mask based on each four bit of the key, balancing 1s and 0s */
        if (i_bit_pos % 4 == 1) 
            {
            i_mask_pos = i_mask_pos + 2;
            tb_work = 1;
            if (fb_worka & fb_key) 
                tb_work = tb_work << i_mask_pos;
            else 
                tb_work = tb_work << (i_mask_pos + 1);
            tb_mask = tb_mask ^ tb_work;
            }
        }
    return EXIT_SUCCESS;
    }


/* c_d( ) is invoked in main */
/* it does the ciphering/deciphering */

int c_d( void )
    {
    /* !! Need to enhance to hold key bits for decipher key checking */
    /* !! when last buffer iobytes less one not working */
    /* !! additional annotation needed */
    enum FilePos 
        {
        NEXT,
        FIRST,
        LAST2,
        LAST
        } filepos;
    int i_last_pos;
    int i_pos;                          /* index to zero last of input buffer */
    unsigned uiisize;                   /* input size */
    unsigned uiosize;                   /* output size */
    unsigned ui_one = 1;                /* constant "1" in bit one */

    i_last_pos = iotwobytes - 1;

    if (build_funct_parms( ) == EXIT_FAILURE) 
        {
        usage( ERBUILDF );
        return EXIT_FAILURE;
        }
    if (build_cipher( ) == EXIT_FAILURE) 
        {
        usage( ERBUILDCIPH );
        return EXIT_FAILURE;
        }
    puibuf = &tba_ibuffer[0];
    puobuf = &tba_obuffer[0];
    filepos = FIRST;
    uiisize = iobytes;                  /* input size */
    uiosize = iobytes;                  /* output size */

    while (1)
        {                               /* read -> write loop */
        uiisize = read_input( uiisize );
        if (task == DECIPH) 
            {
            if (uiisize == 0) 
                {
                filepos = LAST;
                /* to deal with iobyte less one - !! not working properly */
                if ((tba_obuffer[i_last_pos] ^ ui_one) == ui_one) 
                    uiosize = iobytes - 1;
                else 
                    uiosize = tba_obuffer[i_last_pos] / 2;
                }
            } 
        else
            {                           /* ENCIPH */
            if (uiisize != iobytes) 
                {
                filepos = LAST2;
                if (uiisize == (iobytes - 1)) 
                    {
                    tba_ibuffer[i_last_pos] =
                    tba_ibuffer[i_last_pos] | ui_one;
                    } 
                else 
                    {                   /* zero remaining input buffer */
                    for (i_pos = ((uiisize + 1) / 2); i_pos < iotwobytes; i_pos++)
                        tba_ibuffer[i_pos] = 0;
                    tba_ibuffer[i_last_pos] = uiisize * 2;
                    }
                }
            }

        if (filepos == FIRST) 
            filepos = NEXT;
        else 
            {
            uiosize = write_out( uiosize );
            if (filepos == LAST) 
                break;
            }

        if (transpose( ) == EXIT_FAILURE) 
            {
            usage( ERTRANSPOSE );
            return EXIT_FAILURE;
            }
        /* for encipher only */
        if (filepos == LAST2) 
            {
            uiosize = write_out( uiosize );
            break;
            }
        }
    return EXIT_SUCCESS;
    }


/* error handling routine */

void usage( int i_message )
    {
    if (errno != 0) 
        perror( &ca_err_msg[ERPERROR][0] );
    fprintf( stderr, ca_err_msg[i_message] );
    exit( EXIT_FAILURE );               /* stop program in tidy manner */
    }


/* iprintf() prints out input prompts */

void iprintf( int i_inp_msg )
    {
    saverrno = printf( ca_inp_msg[i_inp_msg] );
    fflush( stdout );
    if (saverrno < 0) 
        usage( ERPRINT );
    }


int ciphelp( void )
    {
    int index;
    for (index = 0; index < 16; index++)
        {
        saverrno = printf( ca_help_msg[index] );
        if (saverrno < 0) 
            usage( ERPRINT );
        }
    return EXIT_SUCCESS;
    }


void eval_task( char *eval_t )
    {
    if ((strcmp( eval_t, "-h" ) == 0)
            || (strcmp( eval_t, "-H" ) == 0)
            || (strcmp( eval_t, "/h" ) == 0)
            || (strcmp( eval_t, "/H" ) == 0)
            || (strcmp( eval_t, "h" ) == 0)
            || (strcmp( eval_t, "H" ) == 0)) 
        task = HELP;
    if ((strcmp( eval_t, "-e" ) == 0)
            || (strcmp( eval_t, "-E" ) == 0)
            || (strcmp( eval_t, "/e" ) == 0)
            || (strcmp( eval_t, "/E" ) == 0)
            || (strcmp( eval_t, "e" ) == 0)
            || (strcmp( eval_t, "E" ) == 0)) 
        task = ENCIPH;
    if ((strcmp( eval_t, "-d" ) == 0)
            || (strcmp( eval_t, "-D" ) == 0)
            || (strcmp( eval_t, "/d" ) == 0)
            || (strcmp( eval_t, "/D" ) == 0)
            || (strcmp( eval_t, "d" ) == 0)
            || (strcmp( eval_t, "D" ) == 0)) 
        task = DECIPH;
    }


char *cp_fgets( char *cp_buf, int i_len )
    {
    /* !! consider replacing, alternatives? O/S Specific? */

    int ix;                             /* index used replacing new line charachter with null */
    char *cp_in;
    char LF = 10 ;
    char NULLCHAR = 0 ;

    cp_in = fgets( cp_buf, i_len, stdin );
    if (cp_keybuf == NULL) 
        usage( ERSTDIN );
    else 
        {
        for (ix = 0; ix < i_len; ix++) 
            {
            if (cp_buf[ix] == LF) 
                {
                cp_buf[ix] = NULLCHAR;
                break;
                }
            }
        if (ix == i_len) 
            usage( ERINPUTCR );
        }
    return cp_in;
    }


unsigned int read_input( unsigned int ui_isize )
    {
    if ((ui_isize = read( iinfile, puibuf, ui_isize )) == -1) 
        {
        usage( ERREADIN );
        return EXIT_FAILURE;
        }
    return ui_isize;
    }


unsigned int write_out( unsigned int ui_osize )
    {
    if ((ui_osize = write( ioutfile, puobuf, ui_osize )) == -1) 
        {
        usage( ERWRITEOUT );
        return EXIT_FAILURE;
        }
    return ui_osize;
    }


void close_in_file( void )
    {
    if (iinfile >= 0)
        {
        if (close( iinfile ) == -1) 
            usage( ERIFILECLOS );
        }
    }


void close_out_file( void )
    {
    if (ioutfile >= 0)
        {
        if (close( ioutfile ) == -1) 
            {
            saverrno = errno;
            usage( EROFILECLOS );
            }
        }
    }


int open_out_file_exists( void )
    {
    ioutfile = open( cp_outfile, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE );
    return ioutfile;
    }


int open_in_file( void )
    {
    iinfile = open( cp_infile, O_BINARY | O_RDONLY );
    return iinfile;
    }


int open_out_file( void )
    {
    ioutfile = open( cp_outfile, O_BINARY | O_WRONLY | O_CREAT | O_EXCL, S_IREAD | S_IWRITE );
    return ioutfile;
    }
