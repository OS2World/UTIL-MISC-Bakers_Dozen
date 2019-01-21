/* $title:      'HYPHEN.H ===== Defines & protos for hyphen =====' */
/* $subtitle:   'Elipse Ltd.  [jms]          Revised: 1992 Jan 28' */


/*--- Listing 1 ------------------------- HYPHEN.H -------------
 * Note that a letter is marked if a hyphen can be inserted IN
 * FRONT of it. Ctype is the type of the array that contains the
 * word, one letter per array element. It doesn't have to be an
 * array of char. Note that a string of ctype must be zero termi-
 * nated, even if ctype isn't a char.                                                        *
 *
 * The CHAR macro isolates the character portion of the word. In
 * this case the character is the bottom seven bits. It can actu-
 * ally be anywhere in the ctype, but CHAR must isolate and right
 * justify it.
 *
 * Finally, ISCHAR evaluates to true if it's argument is a legi-
 * timate character (as compared to a font-change request,
 * or whatever)
 *-------------------------------------------------------------*/

#define HYPHEN          0x80
#define HYPHENATE(c)    ( (c) |=  HYPHEN )
#define UNHYPHENATE(c)  ( (c) &= ~HYPHEN )
#define HAS_HYPHEN(c)   ( (c) &   HYPHEN )

/* THE FOLLOWING MAY NOT HAVE SIDE EFFECTS */
#define CHAR(c)     ((c) &  0x7f)
#define ISCHAR(c)   isalpha(CHAR(c))

typedef char ctype;

extern int exception( ctype *word, ctype *end ); 	/* in except.c */
extern int hyphen( ctype *beg, ctype *end ); 	  	/* in hyphen.c */
