/* $title:      'HYPHEN.C ===== Split words at line end =====' */
/* $subtitle:   'Elipse Ltd.  [jms]      Revised: 1993 Jan 03' */


/* #define DEBUG 1 */

/*--- Listing 2 -------------------------- HYPHEN.C ------------
 * An implementation of Knuth's  hyphenation algorithm
 * Compiler:    Borland C/C++
 * Author:      Allen I. Holub
 * Compile-time switches:
 *          DEBUG - if defined, a test driver is provided.
 *                  When DEBUG is defined, the variables
 *                  Debug and Verbose control the display
 *                  of debugging information.
 *
 * (C) 1988, 1991, Allen I. Holub. All rights reserved.
 *------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "hyphen.h"

static void  consonants( ctype *beg,   ctype *end      );
static int   isweird( ctype x,      ctype y, ctype *p );
static int   next( int cur_state, ctype cur_char  );
static ctype nextch( ctype **pp,   ctype *endp     );
static void  phyphen( ctype *start, ctype *end      );
static ctype *prefix( ctype *beg,   ctype *end      );
static ctype *suffix( ctype *beg,   ctype *end      );

#ifdef DEBUG          /* If > 1, then the hyphenated words */
    int  Verbose = 2;                   /* are printed as they are parsed.   */

    static ctype *sgets( char *prompt, ctype *buf, int maxch );
    static void  bprint( ctype *b , ctype *e );
#else
    int Verbose = 0;
#endif

/*--------------------------------------------------------------
 * if THING is #defined then words like "anything" will be split
 * any-thing. On the other hand, if it is defined, then bathing
 * is split ba-thing. You can't have every-thing. In the present
 * version most of the -thing words are in the exception list, so
 * THING can stay undefined.
 *
 * #define THING
 */
/*---------------------------------------------------------------
 * Various pseudo-subroutines. HYPHEN defines a bit to set when a
 * hyphen is inserted. HYPHENATE sets the bit, UNHYPHENATE clears
 * it, HAS_HYPHEN tests for it. The ER macro checks for an "er"
 * at the end of a word, it's used by the consonant-pair checking
 * routine. Is consonant returns true if c is a consonant.
 */
#define ER(p, end) (CHAR(p[0])=='e' && \
                   CHAR(p[1])=='r' && (p+1)==end)

/* The diphthongs ch, gh, ph, sh and th are treated as single
 * consonants. The subroutine nextch() will map these two
 * characters into a single character as follows:
 */

#define CH ('z' + 1 )   /* {     0x7b  \173    */
#define GH ('z' + 2 )   /* |     0x7c  \174    */
#define PH ('z' + 3 )   /* }     0x7d  \175    */
#define SH ('z' + 4 )   /* ~     0x7e  \176    */
#define TH ('z' + 5 )   /* DEL   0x7f  \177    */

/*-------------------------------------------------------------
 * State machine tables used by hyphen.c:
 *
 * There are two types of states. If the first entry is 0 then
 * the state is an array holding the next state, indexed by the
 * current input character. If the first entry is non-zero then
 * the table is a list of char/next-state pairs. The first byte
 * is a character and the second is the state to go to on that
 * character. The leftmost byte in the list is the number of
 * entries in the list. All unspecified transitions are to zero.
 * -------------------------------------------------------------
 * S(x) is used to initialize the state tables. It turned out to
 * be easier to use a define than a typedef.
 */

        /* Points to table for current state machine  */
static char **States;

#define S(x)  static char (x)[]

S(s0 ) = { 0, 84, 84, 84, 1, 1, 84, 84, 84, 84, 84, 84, 84, 84,
         84, 84, 84, 84, 84, 1, 84, 84, 84, 84, 84, 84, 84 };
S(s1 ) = { 0, 0, 0, 2, 0, 7, 0, 21, 0, 0, 0, 0, 23, 0, 30, 0, 0, 0,
         33, 38, 45, 0, 0, 0, 0, 49, 0};
S(s2 ) = { 1, 'i', 3  };
S(s3 ) = { 1, 'p', 4  };
S(s4 ) = { 1, 'o', 5  };
S(s5 ) = { 1, 'c', 6  };
S(s6 ) = { 1, 's', 87 };
S(s7 ) = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 4, 0, 10,
         0, 16, 0, 15, 0, 0, 0, 19 };
S(s8 ) = { 1, 'b', 9  };
S(s9 ) = { 1, 'a', 55 };
S(s10) = { 2, 'e', 11, 'u', 14 };
S(s11) = { 1, 'h', 12 };
S(s12) = { 1, 'p', 6  };
S(s13) = { 2, 'n', 81, 'r', 81 };
S(s14) = { 1, 't', 87 };
S(s15) = { 1, 'i', 14 };
S(s16) = { 1, 'a', 17 };
S(s17) = { 2, 'c', 18, 'l', 18 };
S(s18) = { 0, 88, 0, 0, 0, 88, 0, 0, 0, 88, 0, 0, 0, 0, 0, 88, 0, 0,
         0, 0, 0, 88, 0, 0, 0, 88, 0};
S(s19) = { 1, 'i', 20 };
S(s20) = { 1, 'l', 80 };
S(s21) = { 1, 'n', 22 };
S(s22) = { 1, 'i', 86 };
S(s23) = { 2, 'a', 25, 'u', 24 };
S(s24) = { 1, 'f', 83 };
S(s25) = { 0, 0, 0, 87, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 27, 0, 0, 0, 0,
         0, 87, 0, 0, 0, 0, 0, 0 };
S(s26) = { 2, 't', 87, 'c', 87 };
S(s27) = { 1, 'o', 28 };
S(s28) = { 1, 'i', 29 };
S(s29) = { 1, 't', 89 };
S(s30) = { 1, 'o', 31 };
S(s31) = { 1, 'i', 32 };
S(s32) = { 2, 't', 87, 'c', 6 };
S(s33) = { 1, 'e', 34 };
S(s34) = { 1, 'h', 35 };
S(s35) = { 1, 'p', 36 };
S(s36) = { 1, 'a', 37 };
S(s37) = { 1, 'r', 87 };
S(s38) = { 2, 's', 39, 'u', 41 };
S(s39) = { 1, 'e', 40 };
S(s40) = { 2, 'l', 83, 'n', 83 };
S(s41) = { 1, 'o', 42 };
S(s42) = { 1, 'i', 43 };
S(s43) = { 1, 'c', 44 };
S(s44) = { 0, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
         88, 88, 88, 88, 88, 87, 88, 88, 88, 88, 88, 88, 88 };
S(s45) = { 1, 'n', 46 };
S(s46) = { 1, 'e', 47 };
S(s47) = { 0, 0, 0, 0, 87, 0, 0, 0, 0, 48, 0, 0, 0, 83, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0 };
S(s48) = { 1, 'c', 87 };
S(s49) = { 0, 0, 0, 0, 0, 0, 0, 50, 35, 0, 0, 0, 83, 0, 0, 0, 0, 0, 51,
         0, 0, 0, 0, 0, 0, 0, 0 };
S(s50) = { 1, 'o', 87 };
S(s51) = { 1, 'a', 52 };
S(s52) = { 1, 'n', 53 };
S(s53) = { 0, 88, 88, 88, 88, 81, 88, 88, 88, 88, 88, 88, 88, 88,
         88, 54, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88 };
S(s54) = { 1, 'i', 82 };
S(s55) = { 0, 0, 0, 0, 0, 80, 0, 0, 80, 80, 0, 80, 80, 0, 0, 80, 0, 0,
         0, 0, 13, 80, 80, 80, 80, 80, 0};
static char *Suffixes[] =
{
        s0,  s1,  s2,  s3,  s4,  s5,  s6,  s7,  s8,  s9,
        s10, s11, s12, s13, s14, s15, s16, s17, s18, s19,
        s20, s21, s22, s23, s24, s25, s26, s27, s28, s29,
        s30, s31, s32, s33, s34, s35, s36, s37, s38, s39,
        s40, s41, s42, s43, s44, s45, s46, s47, s48, s49,
        s50, s51, s52, s53, s54, s55
};
/*-------------------------------------------------------------*/
S(p0 ) = {1, 0, 0 };
S(p1 ) = {0, 0, 2, 4, 6, 9, 0, 0, 13, 22, 0, 0, 26, 29, 39, 41, 45,
         50, 0, 53, 57, 64, 0, 0, 0, 0, 0 };
S(p2 ) = {1, 'e', 3 };
S(p3 ) = {0, 0, 0, 81, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81,
         0, 0, 0, 81, 0, 0, 0 };
S(p4 ) = {1, 'o', 5 };
S(p5 ) = {2, 'm', 87, 'n', 87 };
S(p6 ) = {1, 'i', 7 };
S(p7 ) = {1, 's', 8 };
S(p8 ) = {0, 85, 85, 85, 85, 85, 85, 85, 0, 85, 85, 85, 85, 85,
         85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 85 };
S(p9 ) = {2, 'q', 10, 'x', 87 };
S(p10) = {1, 'u', 11 };
S(p11) = {1, 'i', 12 };
S(p12) = {0, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
         81, 81, 81, 81, 81, 81, 81, 81, 0, 81, 81, 81, 81 };
S(p13) = {0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 19, 0 };
S(p14) = {1, 'n', 15 };
S(p15) = {1, 'd', 87 };
S(p16) = {1, 'r', 17 };
S(p17) = {1, 's', 18 };
S(p18) = {1, 'e', 87 };
S(p19) = {1, 'p', 20 };
S(p20) = {1, 'e', 21 };
S(p21) = {1, 'r', 84 };
S(p22) = {2, 'n', 69, 'm', 86 };
S(p23) = {0, 81, 0, 0, 0, 0, 81, 81, 0, 0, 0, 0, 81, 81, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0 };
S(p24) = {2, 'e', 21, 'r', 25 };
S(p25) = {1, 'o', 87 };
S(p26) = {1, 'e', 27 };
S(p27) = {1, 'x', 35 };
S(p28) = {1, 'i', 87 };
S(p29) = {0, 30, 0, 0, 0, 0, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 37, 0, 0, 0, 0, 0 };
S(p30) = {0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         33, 0, 0, 0, 35, 0, 0 };
S(p31) = {1, 'r', 32 };
S(p32) = {1, 'o', 83 };
S(p33) = {1, 'h', 34 };
S(p34) = {1, 'e', 82 };
S(p35) = {1, 'i', 82 };
S(p36) = {1, 'n', 35 };
S(p37) = {1, 'l', 38 };
S(p38) = {1, 't', 65 };
S(p39) = {1, 'o', 40 };
S(p40) = {1, 'n', 86 };
S(p41) = {2, 'u', 42, 'v', 43 };
S(p42) = {1, 't', 87 };
S(p43) = {1, 'e', 44 };
S(p44) = {1, 'r', 86 };
S(p45) = {1, 's', 46 };
S(p46) = {1, 'e', 47 };
S(p47) = {1, 'u', 48 };
S(p48) = {1, 'd', 49 };
S(p49) = {1, 'o', 83 };
S(p50) = {1, 'u', 51 };
S(p51) = {1, 'a', 52 };
S(p52) = {1, 'd', 87 };
S(p53) = {0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0,
         0, 0, 56, 0, 0, 0, 0, 0 };
S(p54) = {1, 'm', 28 };
S(p55) = {1, 'm', 18 };
S(p56) = {2, 'b', 87, 'p', 20 };
S(p57) = {2, 'h', 58, 'r', 61 };
S(p58) = {1, 'e', 59 };
S(p59) = {1, 'r', 60 };
S(p60) = {1, 'e', 87 };
S(p61) = {2, 'a', 62, 'i', 68 };
S(p62) = {1, 'n', 63 };
S(p63) = {1, 's', 23 };
S(p64) = {1, 'n', 66 };
S(p65) = {1, 'i', 83 };
S(p66) = {0, 85, 85, 85, 70, 85, 85, 85, 85, 0, 85, 85, 85, 85,
         85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85 };
S(p67) = {1, 'r', 87 };
S(p68) = {0, 81, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 81, 0, 0, 0, 0, 0 };
S(p69) = {0, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,
         85, 85, 85, 85, 85, 85, 24, 85, 85, 85, 85, 85, 85 };
S(p70) = {1, 'e', 67 };

static char *Prefixes[] =
{
        p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9,
        p10, p11, p12, p13, p14, p15, p16, p17, p18, p19,
        p20, p21, p22, p23, p24, p25, p26, p27, p28, p29,
        p30, p31, p32, p33, p34, p35, p36, p37, p38, p39,
        p40, p41, p42, p43, p44, p45, p46, p47, p48, p49,
        p50, p51, p52, p53, p54, p55, p56, p57, p58, p59,
        p60, p61, p62, p63, p64, p65, p66, p67, p68, p69,
        p70
};

/*-------------------------------------------------------------*/

#ifdef DEBUG
static int  Debug = 0;                  /* True if debug diagnostics
                           are to be printed  */
#endif
/*-------------------------------------------------------------*/
static char Vt[] =
   /* a b c d e f g h i j k l m n o p q r s t u v w x y z */
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 };

#define ISVOWEL(c)      ( islower(CHAR(c)) && Vt[CHAR(c) - 'a'] )
#define ISCONSONANT(c)  ((c) && !ISVOWEL(c))
/*-------------------------------------------------------------*/


static  ctype   *suffix( ctype *beg, ctype *end )
    {
  /*  Split off any suffixes, using the "Suffixes" state table
   *  (defined below) to recognize them.
   */

    ctype    *p, c, c2    ;
    unsigned state, times ;

    state = 1;
    times = 0;
    States = Suffixes;

    for ( p = end;  p >= beg;  )
        {
        state = next( state, c = CHAR(*p--) );

        if ( !('a' <= c && c <= 'z') )  /* Words containing non- */
            return end;                 /* lower-case characters */
                                      /* won't be hyphenated.  */

     /* Test for state 0 with an if instead of in the switch.
      * Lattice C will make the switch very inefficient if the
      * range of case values is too high. State 0 is weird;
      * we use it to test for trailing e or ed if we've failed.
      * The first time through we set p to end so we can check
      * the end of the word. The third time through we abort.
      * This means that we'll retry if the word ends -s -e -d
      * -ed -de -dd -ee. The extra endings shouldn't matter.
      */

        if ( state == 0 )
            {
            if ( times == 0 )
                p = end;                /* Strip trailing e or d */

            else if ( times == 1 )      /* Strip trailing ed, de */
                p = end - 1;            /* ee, or dd             */
            else
                return end;

            times++;
            }

        switch ( state)
            {
        case 86:
             /* Process -ing. This could have been done with
              * more states but it seemed easier to do it here.
              * I've modified the Knuth algorithm to not
              * hyphenate in front of a th that precedes
              * an -ing to make any-thing split right. Note
              * that bathing will split incorrectly though.
              * #define THING to enable this function.
              * On entry p points at the character preceding
              * the 'i'.
              */

            c = CHAR(p[0]  );
            c2 = CHAR(p[-1] );

            if ( p - beg < 3 )
                return end;             /* FAIL */
#ifdef THING
                else if( c == 'h'  &&  c2 == 't' )
                    p -= 2;
#endif
            else if ( (c == c2) && (! ISVOWEL(c))
                && (c != 'f') && (c != 's')
                && (c != 'l') && (c != 'z') )
                {
                --p;
                }
            else if ( c == 'l' && strchr( "bcdfghkptz", c2 ) )
                p -= (c2 == 'k' && CHAR(p[-2]) == 'c') ? 
                    1 : 2;

                /* fall through to state 87 */

        case 87:
            HYPHENATE(*(p+1) );
            return p;

        case 88:
            HYPHENATE(*(p+2) );
            return (p + 1);

        case 89:
            HYPHENATE(*(p+5) );
            HYPHENATE(*(p+1) );
            return p;

        case 82:
            p++;                        /* p += 3 */
        case 81:
            p++;                        /* p += 2 */
        case 80:
            p++;                        /* P += 1 */
        case 83:
            end = p;
            HYPHENATE(*(p + 1) );
            state = 1;
            break;
        case 84:                        /* FAILURE STATE */
            return end;
            }
        }

    return end;
    }


/*-------------------------------------------------------------*/


static ctype *prefix( ctype *beg, ctype *end )
    {
  /* Split off any prefixes, using the "Prefixes" state table */

    ctype *p;
    int   state = 1;

    States = Prefixes;
    for ( p = beg;  p < end;  )
        {
        switch ( state = next( state, CHAR(*p++) ))
            {
        case 82:
            HYPHENATE(*p );
            HYPHENATE(*(p-1) );
            return p;
        case 83:
            HYPHENATE(*p );
            HYPHENATE(*(p-2) );
            return p;
        case 84:
            HYPHENATE(*p );
            HYPHENATE(*(p-3) );
            return p;

        case 81:
            --p;
        case 87:
            HYPHENATE(*p );
            return p;

        case 85:
            --p;
        case 86:
            beg = p;
            state = 1;
        case 69:
            HYPHENATE(*p );
            break;

        case 70:
            HYPHENATE(*(p-1) );
            break;

        /*      The following states don't actually exist.
         *      Putting them in the table causes the compiler
         *      to compile the switch more efficiently.
         */

        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78:
        case 79:
            break;
            }
        }
    return beg;                         /* Failure */
    }

/*--------------------------------------------------------------
 * This table is used to find exceptions to the VCCV rule. It is
 * indexed by the first of the two consonants & contains pointers
 * to strings, any character of which will form an exception if
 * it's the second consonant.
 */

static char    *vccv_except[] =
{
    /* a  */    ""      ,
    /* b  */    "lr"    ,
    /* c  */    "lr"    ,
    /* d  */    "gr"    ,
    /* e  */    ""      ,
    /* f  */    "lr"    ,
    /* g  */    "lr"    ,
    /* h  */    ""      ,
    /* i  */    ""      ,
    /* j  */    ""      ,
    /* k  */    "n"     ,
    /* l  */    "kq"    ,
    /* m  */    ""      ,
    /* n  */    "{kx"   , /* CH, k, x }*/
    /* o  */    ""      ,
    /* p  */    "lr"    ,
    /* q  */    ""      ,
    /* r  */    "k"     ,
    /* s  */    "pq"    ,
    /* t  */    "{r"    , /* CH, r    }*/
    /* u  */    ""      ,
    /* v  */    ""      ,
    /* w  */    "hlnr"  ,
    /* x  */    ""      ,
    /* y  */    ""      ,
    /* z  */    ""      ,
    /* CH */    "lr"    ,
    /* GH */    "t"     ,
    /* PH */    "r"     ,
    /* SH */    ""      ,
    /* TH */    "r"
};
/*-------------------------------------------------------------*/


static ctype nextch( ctype **pp, ctype *endp )
    {
    /*  Advance *pp (one for a vowel or consonant, 2 for a
     *  diphthong) and return the character we've just skipped.
     *  Diphthongs are mapped to the single characters: TH, SH,
     *  PH, CH or GH. Returns 0 once *pp reaches or passes endp.
     */

    ctype *p = *pp;
    ctype rval = CHAR(*p++);

    if ( *pp > endp )
        return (ctype) 0;

    if ( CHAR(*p) == 'h' )
        {
        switch ( (int) rval)
            {
        case 't':
            rval = TH;
            p++;
            break;
        case 's':
            rval = SH;
            p++;
            break;
        case 'p':
            rval = PH;
            p++;
            break;
        case 'c':
            rval = CH;
            p++;
            break;
        case 'g':
            rval = GH;
            p++;
            break;
            }
        }
    *pp = p;
    return rval;
    }


/*-------------------------------------------------------------*/


static int isweird( ctype x, ctype y, ctype *p )
    {
    /* Return true if the string pointed to by p ends in:
     *  XYer XYers XYage XYages XYest
     *
     * where XY is one of the pairs:
     *  ft ld mp nd ng ns nt rg rm rn rt st
     */

    unsigned c1, c2, c3;

    c1 = CHAR(*p++);
    c2 = CHAR(*p++);
    c3 = CHAR(*p  );
    x = CHAR(x);
    y = CHAR(y);

    return (((c1 == 'e' && c2 == 'r')
        || (c1 == 'a' && c2 == 'g' && c3 == 'e')
        || (c1 == 'e' && c2 == 's' && c3 == 't')
        )
        &&
        ((x == 'f' && y == 't')
        || (x == 'l' && y == 'd')
        || (x == 'm' && y == 'p')
        || (x == 's' && y == 't')
        || (x == 'n' && strchr( "dgst", CHAR(y) ))
        || (x == 'r' && strchr( "gmnt", CHAR(y) ))
        )
        );
    }


/*-------------------------------------------------------------*/


static  void    consonants( ctype *beg, ctype *end )
    {
    /*  Hyphenate consonant pairs.
     *  Look for a VCC pattern: use the state machine:
     *
     *           V          C        C
     * Start ÄÄÂÄ> 1 ÄÄÄÄÄ> 2 ÄÄÄÂÄ> 3 ÄÄÄÂÄ> 4
     *     ³   ³C    ³ V    ³    ³ V      ³
     *     ³   ³     ³      ³    ³        ³  (no fetch )
     *     ³   v     ³      v    ³        v
     *     ÃÄ<ÄÙ     ÀÄÄÄÄÄÄ<ÄÄÄÄÙ        ³
     *     ³                              ³
     *     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ<ÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
     *
     *  The machine is implemented explicitly (with whiles, ifs,
     *  and such) rather than with a table-driven engine.
     *  A new character is fetched in states 1, 2, and 3 (but
     *  not 4). When you exit (reach state 4) c1 and c2 will hold
     *  the two consonants, cp will point to the beginning of the
     *  second consonant, beg will point just past the second
     *  consonant.
     */

    ctype  c1, c2, *cp;
    char *p;

    while ( 1 )
        {
state1:

        do  {
            c2 = nextch( &beg, end );
            }
        while ( ISCONSONANT(c2) );

        do  {
            for ( c1 = c2;  ISVOWEL(c1);  )
                {
                cp = beg;
                c1 = nextch( &beg, end );
                }
            if ( c1 == 'q' && *beg == 'u' ) /*  Vqu */
                {
                HYPHENATE(*cp );        /*         V-qu */
                nextch( &beg, end );    /*   skip the u */
                goto state1;
                }
            cp = beg;
            c2 = nextch( &beg, end );

            }
        while ( ISVOWEL(c2) );

        if ( !c1 || !c2 )
            break;

        /* At this point we have found a Vowel-Cons-Cons
         * sequence. C1 and c2 hold the left and right
         * consonant respectively. Beg points at the
         * character following the second consonant.
         * Cp points at the second consonant.
         */

        if ( c1 == 'c' && c2 == 'k' )   /*        Vck  */
            {
            if ( *beg )
                HYPHENATE(*beg );       /*        Vck- */
            }
        else if ( c1 == c2 )            /*     ller(s) */
            {
            if ( (c1 != 'l' && c1 != 's') || (ISVOWEL(*beg) &&
                ! ER(beg, end) ) )
                HYPHENATE(*cp );
            }
        else if ( ISVOWEL(*beg ) )      /*        VCCV */
            {
            if ( !isweird( c1, c2, beg ) )
                {
                if ( !((p = vccv_except[(int) c1 - 'a'])
                    && strchr( (char *) p, (int) c2 )) )
                    HYPHENATE(*cp );
                }
            }
        }
    UNHYPHENATE(*end );
    }


/*-------------------------------------------------------------*/


static  void    phyphen( ctype *start, ctype *end )
    {
    /*  Print a word with all the hyphens visible */

    for ( ;*start && start <= end;  putchar( CHAR(*start++) ) )
        if ( HAS_HYPHEN(*start) )
            putchar( '-' );

    putchar( '\n' );
    }


/*-------------------------------------------------------------*/


int hyphen( ctype *beg, ctype *end )
    {
    /*  Hyphenate the word delineated by beg and end: First
     *  strip suffixes, then strip a trailing s e or ed,
     *  then strip prefixes. Only words with more than four
     *  letters and which consist of lower case letters only
     *  (no NULLs) will be hyphenated. 0 is returned if no
     *  attempt was made to hyphenate the word, 1 otherwise.
     *  If the word has already been hyphenated, 1 is returned
     *  but the word is not modified.
     */

    ctype *prefixp, *suffixp;
    int   c;

    if ( end - beg <= 4 )
        return 0;

    for ( prefixp = beg;  prefixp <= end;  prefixp++ )
        {
        if ( HAS_HYPHEN(*prefixp) )
            return 1;
        if ( !islower( CHAR(*prefixp) ) || ! ISCHAR(*prefixp) )
            return 0;
        }
    if ( !exception( beg, end ) )
        {
        suffixp = suffix( beg, end );   /* Hyphenate and remove */
                                     /* any suffixes.        */
        if ( suffixp == end )
            {
            /*  If root has a trailing s e or ed remove it
             *  before applying any other rules
             */
            c = CHAR(*end);

            if ( c == 's' || c == 'e' )
                suffixp = end - 1;
            else if ( CHAR(end[-1]) == 'e' && c == 'd' )
                suffixp = end - 2;
            }
        prefixp = prefix( beg, suffixp );   /* Hyphenate and */
                                     /* remove any prefixes  */
#ifdef DEBUG
      /*------------------------------------------------------*/
      /*  Print the word in the form:  prefixes/root/suffixes */
      /*                                                      */
        bprint( beg, prefixp - 1 );     /* Print prefixes       */
        putchar( '/' );
        bprint( prefixp, suffixp );     /* Print middle         */
        putchar( '/' );
        bprint( suffixp + 1, end );     /* Print suffixes       */
        printf( "=\t" );
      /*------------------------------------------------------*/
#endif
        if ( (suffixp - prefixp) >= 3 )
            {
            /* Apply the consonant pairs rules only if the word
             * has at least 4 letters in it (after prefixes
             * and suffixes have been removed
             */
            consonants( prefixp, suffixp );
            }
        }
    if ( Verbose > 1 )
        phyphen( beg, end );
    return 1;
    }


/*-------------------------------------------------------------*/


static  int next( int cur_state, ctype cur_char )
    {
    /*  Given the current state and the current input character
     *  return the next state. The global variable States must
     *  point at the correct state table (Suffixes or Prefixes).
     */

    char *p = States[cur_state];
    int rval, i, c ;

    c = CHAR(cur_char);

#ifdef DEBUG
    if ( Debug )
        printf( "%s: Current state = %d, Input char = <%c>, ",
            States == Prefixes ? "PREFIX" : "SUFFIX",
            cur_state, c );
#endif

    if ( !*p )
        rval = (int) (p[(c - 'a') + 1]);
    else
        {
        for ( rval = 0,  i = *p++;  --i >= 0;  p += 2 )
            if ( c == p[0] )
                {
                rval = p[1];
                break;
                }
        }

#ifdef DEBUG
    if ( Debug )
        printf( "Next state = %d\n", rval );
#endif
    return (rval);
    }


/*===========================================================*/
/*   MISC. DEBUGGING ROUTINES (INCLUDING A MAIN MODULE):     */

#ifdef DEBUG

static void bprint( ctype *b , ctype *e )
    {
    /* Print all characters between b and e, inclusive.
     * Can't use printf because it expects char* for %s, not
     * CYPTE*.
     */

    while ( b <= e && *b )
        putchar( CHAR(*b++) );
    }


/*-----------------------------------------------------------*/


static ctype *sgets( char *prompt, ctype *buf, int maxch )
    {
    /* Print prompt to stdout, then fill buf fm stdin. Get at
     * most maxch characters. Return pointer to string terminator
     * (i.e., the null at the end of the string) or 0 on end of
     * file. We set a high bit to make sure that only the bottom
     * seven bits of the character are used in the algorithm.
     */

    int c;
    ctype *start;

    start = buf;
    printf( (char *) prompt );

    while ( (c = getchar( )) != EOF && !isalpha( c ) )
        ;

    if ( c == EOF )
        return NULL;
    else
        *buf++ = c;

    while ( (c = getchar( )) != EOF &&
        !isspace( c ) && --maxch > 0 )
        *buf++ = c;
    *buf = 0;
    return (c == EOF && start == buf) ? NULL : buf;
    }


/*------------------------------------------------------------*/


int main(int argc, char *argv[] )
    {
    ctype buf[132], *p;

    /* Note that ctype should be char for this to work */

    if ( argc > 1 )
        Debug = 1;

    while ( p = sgets( "word: ", buf, 132 ) )
        {
        if ( !*buf )
            printf( "\n" );
        else if ( p-- > buf )
            hyphen( buf, p );
        }
    return 0;
    }

#endif /* DEBUG */
