/*
 * C2ENG.C
 *
 * A small utility to print equivalent English for a C language declaration.
 *
 * Usage:
 *
 *      c2eng <decl;> [...]
 *
 * Version 1.0 - Initial revision
 *
 * Will C. Crowder
 * April 26, 1990
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dir.h>
#include <ctype.h>


/* DEFINES: */

#define TRUE            1                           /* boolean */
#define FALSE           0                           /* boolean */
#define YES             TRUE                        /* boolean */
#define NO              FALSE                       /* boolean */

#define MAXDECL         512                         /* maximum declaration length */
#define MAXENGLISH      1024                        /* maximum English length */
#define MAXWORD         64                          /* maximum word length */

                                                    /* c_to_english() return codes */
#define C2E_OK          0                               /* all OK */
#define C2E_OVERFLOW    1                               /* buffer overflow */
#define C2E_BADPARENS   2                               /* unbalanced parentheses */
#define C2E_SYNTAX      3                               /* syntax error */
#define C2E_BADBRACES   4                               /* unbalanced braces */


/* PROTOTYPES: */

int main(int argc,char *argv[]);
void error(char *msg,...);
int add_to_buffer(char *buf,size_t size,char *fmt,...);
int c_to_english(char *decl,char *english,size_t size);
char *word_right(char **buf);
char *word_left(char *base,char **buf);
int is_typeword(char *word);
void usage(void);
void print_words(int len,int indent,char *string);


/* GLOBAL DATA: */

char pname[MAXFILE];                                /* program name */
char *twords[] = {                                  /* C type keywords */
    "auto",
    "static",
    "extern",
    "register",
    "char",
    "short",
    "int",
    "long",
    "float",
    "double",
    "unsigned",
    "signed",
    "enum",
    "struct",
    "union",
    "const",
    "near",
    "far",
};
int ntwords = sizeof(twords)/sizeof(twords[0]);     /* # of type keywords */


/*
 * main     - entry point, main functions
 *
 * int main(int argc,char *argv[]);
 *
 * This is the entry point.
 *
 * Usage:
 *
 *      c2eng <decl;> [...]
 */

int main(int argc,char *argv[])
{

    static char decl[MAXDECL];              /* declaration buffer */
    static char english[MAXENGLISH];        /* English buffer */

    int i;                                  /* index counter */
    int nc;                                 /* # of chars added to decl */

    int did_one = NO;                       /* flag that says we printed something */


    /* get program name */

    fnsplit(argv[0],NULL,NULL,pname,NULL);
    strlwr(pname);


    /* form each declaration and translate it */

    nc = 0;
    decl[0] = '\0';

    for (i = 1; i < argc; i++) {

        nc += strlen(argv[i]);                  /* add up chars */

        if (decl[0] != '\0')                    /* adding a space as well */
            nc++;

        if (nc >= MAXDECL)                      /* check length */
            error("declaration too long");

        if (decl[0] == '\0')                    /* do it */
            strcpy(decl,argv[i]);
        else {
            strcat(decl," ");
            strcat(decl,argv[i]);
        }

        if (*(argv[i] + strlen(argv[i]) - 1) == ';') {

            /* 
             * That's the decl, say we got one and print it
             */

            did_one = YES;

            printf("%s:\n",decl);

            /*
             * Get the English and switch on return code
             */

            switch (c_to_english(decl,english,MAXENGLISH)) {

            case C2E_OK:        /* all ok */

                print_words(80,5,english);
                break;

            case C2E_OVERFLOW:

                printf("Error: English buffer overflow\n\n");
                break;

            case C2E_BADPARENS:

                printf("Error: unbalanced parentheses\n\n");
                break;

            case C2E_SYNTAX:

                printf("Error: declaration syntax error\n\n");
                break;

            case C2E_BADBRACES:

                printf("Error: unbalanced braces\n\n");
                break;

            }

            decl[0] = '\0';
            nc = 0;

        }

    }

    if (!did_one)       /* didn't do anything */
        usage();            /* so print usage */

    return(0);

}


/*
 * c_to_english - translate C declaration to English
 *
 * int c_to_english(char *decl,char *english,size_t size);
 *
 * This function translates the C language declaration pointed to by decl
 * and places the translation in the area pointed to by english.  size
 * is the size of the english buffer.
 *
 * Returns C2E_OK if all ok, else
 *
 *      C2E_OVERFLOW    - english buffer overflowed
 *      C2E_BADPARENS   - unbalanced parentheses
 *      C2E_SYNTAX      - declaration syntax error
 *      C2E_BADBRACES   - unbalanced braces
 */

int c_to_english(char *decl,char *english,size_t size)
{

    char *right,*left;              /* word pointers */
    char *p,*q;                     /* general purpose char pointers */
    char *rp,*lp;                   /* left and right pointers for word functions */
    char temp;                      /* temporary character storage */
    enum {none,emit_left,           /* last emission--left or right? */
      emit_right} last = none;
    int i;                          /* general purpose integer */
    int ptr_to = FALSE;             /* pending "pointer to" emission */
    enum why {parens, done,         /* why done with traversal */
      not_done} why_right = not_done,
      why_left = not_done;

    /*
     * Initialize english buffer
     */

    english[0] = '\0';

    /*
     * Find the identifier.  This is the first non-type keyword not following
     * "struct", "union" or "enum."
     */

    rp = decl;

    while (*(p = word_right(&rp)) != '\0' && *p != ';') {

        /*
         * If this is "struct", "union" or "enum", eat the next and continue
         */

        if (strcmp(p,"struct") == 0 || strcmp(p,"union") == 0 || strcmp(p,"enum") == 0) {
            word_right(&rp);
            continue;
        }

        /*
         * See if it's a valid identifier
         */

        for (q = p; *q != '\0'; q++)
            if (*q != '_' && !isalnum(*q))
                break;

        if (*q != '\0')
            continue;       /* not valid identifier */

        /*
         * Was valid identifier, if not a type keyword, that's it
         */

        if (!is_typeword(p))
            break;

    }

    /*
     * If we didn't find identifier, syntax error
     */

    if (*p == '\0' || *p == ';')
        return(C2E_SYNTAX);

    /*
     * Got identifier, emit it and initialize left and right
     */

    if (add_to_buffer(english,size,"%s is ",p) == 0)
        return(C2E_OVERFLOW);

    /*
     * Set up left and right pointers
     */

    lp = rp;
    word_left(decl,&lp);

    right = word_right(&rp);
    left = word_left(decl,&lp);


    /*
     * Traverse and emit declaration.  Follow path right until end of
     * decl or right parens.  Then traverse left until left parens
     * or beginning, then traverse right, etc., until all words have
     * been processed.
     */

    while (why_right != done && why_left != done) {

        /*
         * Traverse right until ')' or end
         */

        while (why_right != done) {

            /*
             * If word starts with ')', why_right = parens and break,
             * else if word is empty, we're done.
             * else just continue.
             */

            if (*right == ')') {
                why_right = parens;         /* say we terminated on parens */
                right = word_right(&rp);    /* get next word */
                break;                      /* and go to left */
            } else if (*right == '\0' || *right == ';') {
                why_right = done;
                break;
            }

            if (*right == '[') {

                /* 
                 * "array of".  See if we have size, and emit it if we
                 * do.  Otherwise, just add it and continue.
                 */

                if (*rp == ']') {

                    /* no size, just emit "array of " and continue */

                    if (add_to_buffer(english,size,"array of ") == 0)
                        return(C2E_OVERFLOW);

                    rp++;

                } else {

                    /* have size, save beginning, find end */

                    p = rp;

                    while (*rp != ']' && *rp != ';' && *rp != '\0')
                        rp++;

                    if (*rp != ']')     /* unbalanced braces */
                        return(C2E_BADBRACES);

                    /* terminate at end, emit, and restore terminator */

                    temp = *rp;
                    *rp = '\0';

                    if (add_to_buffer(english,size,"array (size %s) of ",p) == 0) {
                        *p = temp;
                        return(C2E_OVERFLOW);
                    }

                    *rp++ = temp;

                }

            } else if (*right == '(') {

                /*
                 * "function returning".  If the last thing we emitted was
                 * also on the right, this is a syntax error.
                 */

                if (last == emit_right)
                    return(C2E_SYNTAX);

                /*
                 * Check for immediate ')' and just emit "function returning"
                 * if found.  Otherwise, emit all between the parens as
                 * the expected arguments.
                 */

                if (*rp == ')') {

                    /* just "function returning" */

                    if (add_to_buffer(english,size,"function returning ") == 0)
                        return(C2E_OVERFLOW);

                    rp++;       /* skip the parens */

                } else {

                    /* find balanced parens, emit all between */

                    i = 1;
                    p = rp;

                    while (i > 0 && *rp != ';' && *rp != '\0') {
                        if (*rp == '(')
                            i++;
                        else if (*rp == ')')
                            i--;
                        rp++;
                    }

                    if (i != 0)         /* unbalanced parentheses */
                        return(C2E_BADPARENS);

                    /*
                     * rp points one past closing parens.  Terminate, emit
                     * and restore.
                     */

                    temp = *rp;
                    *rp = '\0';

                    if (add_to_buffer(english,size,"function expecting (%s returning ",p) == 0) {
                        *rp = temp;
                        return(C2E_OVERFLOW);
                    }

                    *rp = temp;

                }
                
            } else      /* not array or function, error */
                return(C2E_SYNTAX);

            /*
             * Say we emitted on right and get next word
             */

            last = emit_right;
            right = word_right(&rp);

        }

        /*
         * Traverse left until '(' or beginning
         */

        while (why_left != done) {

            if (*left == '(') {
                why_left = parens;          /* terminating on parens */
                left = word_left(decl,&lp); /* and skip the parens */
                break;
            } else if (*left == '\0') {
                why_left = done;
                break;
            }

            if (*left == '*') {

                /*
                 * "pointer to".  If ptr_to is FALSE, just set to TRUE
                 * and keep going.  Otherwise, emit *one* "pointer to",
                 * leave ptr_to TRUE and keep going.
                 */

                if (ptr_to) {
                    if (add_to_buffer(english,size,"pointer to ") == 0)
                        return(C2E_OVERFLOW);
                }

                ptr_to = TRUE;

            } else {

                /*
                 * If ptr_to is TRUE and the word is near or far, emit the
                 * word and "pointer to."  Else, emit all words left from
                 * right to left.
                 */

                if (ptr_to == TRUE && (strcmp(left,"near") == 0 || strcmp(left,"far") == 0)) {

                    if (add_to_buffer(english,size,"%s pointer to ",left) == 0)
                        return(C2E_OVERFLOW);

                    ptr_to = FALSE;

                } else {

                    /*
                     * Check for pending "pointer to" and emit it
                     */

                    if (ptr_to) {
                        if (add_to_buffer(english,size,"pointer to ") == 0)
                            return(C2E_OVERFLOW);
                        ptr_to = FALSE;
                    }

                    /*
                     * lp + strlen(left) points to end of word on left.
                     * Terminate there and then scan left until '(' or
                     * beginning.
                     */

                    lp += strlen(left);
                    temp = *lp;
                    *lp = '\0';
                    p = lp;

                    while (lp != decl && *lp != '(')
                        lp--;

                    /*
                     * If we're pointing at parens and why_right isn't
                     * parens, they're unbalanced.
                     */

                    if (*lp == '(') {
                        if (why_right != parens)
                            return(C2E_BADPARENS);
                        lp++;
                    }

                    if (add_to_buffer(english,size,"%s ",lp) == 0) {
                        *p = temp;
                        return(C2E_OVERFLOW);
                    }

                    /*
                     * Restore the temp and keep going
                     */

                    *p = temp;

                }

            }

            /*
             * Say we emitted on left and check for termination
             */

            last = emit_left;
            left = word_left(decl,&lp);

        }

        /*
         * If ptr_to is still TRUE, we still need to emit "pointer to "
         */

        if (ptr_to) {
            if (add_to_buffer(english,size,"pointer to ") == 0)
                return(C2E_OVERFLOW);
            ptr_to = FALSE;
        }

        /*
         * Check termination reason and make sure it matches with right
         */

        if (why_right != why_left)
            return(C2E_BADPARENS);

    }

    return(C2E_OK);

}


/*
 * error    - emit error message and exit
 *
 * void error(char *msg,...);
 *
 * This function prints msg (printf format string) and the subsequent args
 * to the standard error device, and exits.
 */

void error(char *msg,...)
{

    va_list ap;             /* arg pointer */

    va_start(ap,msg);               /* point at args */

    fprintf(stderr,"%s: ",pname);   /* give program name */
    vfprintf(stderr,msg,ap);        /* and message */
    fprintf(stderr,"\n");           /* newline */

    va_end(ap);                     /* and we're done */

    exit(1);

}


/*
 * add_to_buffer    - add formatted string to buffer
 *
 * int add_to_buffer(char *buf,size_t size,char *fmt,...);
 *
 * The string pointed to by fmt and the following args are passed to
 * sprintf().  The resulting string is appended to buffer if there is
 * room.  If there is no room, returns 0, else, returns 1.
 */

int add_to_buffer(char *buf,size_t size,char *fmt,...)
{

    char temp[256];             /* local temp buffer */
    va_list ap;                 /* argument pointer */

    va_start(ap,fmt);           /* start args */
    vsprintf(temp,fmt,ap);      /* make the string */
    va_end(ap);                 /* done with ap */

    if (strlen(buf) + strlen(temp) + 1 > size)
        return(0);      /* no room */

    strcat(buf,temp);

    return(1);

}


/*
 * word_right   - return word on right
 *
 * char *word_right(char **buf);
 *
 * This function returns the word in *buf to the right.  buf is modified
 * to point at the char just after the word.  The returned pointer points
 * to a static area within the function.
 *
 * A word is a sequence of characters containing only letters, numbers or
 * underscore.  If a character not meeting those constraints is encountered
 * it is returned as a single word.  Whitespace is skipped.
 */

char *word_right(char **buf)
{

    static char word[MAXWORD];              /* word buffer */
    int nc = 0;                             /* # of chars added */

    /*
     * If at end, just return empty word
     */

    if (**buf == '\0') {
        word[0] = '\0';
        return(word);
    }

    /*
     * Skip whitespace
     */

    while (isspace(**buf))
        (*buf)++;

    /*
     * If this char isn't "_" or alnum, just return it
     */

    if (**buf != '_' && !isalnum(**buf)) {
        word[0] = *(*buf)++;
        word[1] = '\0';
        return(word);
    }

    /*
     * Form the entire word
     */

    while (**buf == '_' || isalnum(**buf)) {
        word[nc++] = *(*buf)++;
        if (nc == MAXWORD)
            error("word buffer overflow");
    }

    /*
     * Terminate the word and return it
     */

    word[nc] = '\0';

    return(word);

}


/*
 * word_left    - return word on left
 *
 * char *word_left(char *base,char **buf);
 *
 * This function returns the word to the left in *buf, where base points to
 * the beginning of the buffer.  The returned pointer points to a static
 * area within the function.  On return, *buf points to the beginning of
 * the word.
 *
 * A word is a sequence of characters containing only letters, numbers or
 * underscore.  If a character not meeting those constraints is encountered
 * it is returned as a single word.  Whitespace is skipped.
 */

char *word_left(char *base,char **buf)
{

    static char word[MAXWORD];              /* word buffer */
    char *p;                                /* marks end of word */
    char temp;                              /* temp char storage */


    /*
     * If *buf == base, we're at the far left
     */


    if (*buf == base) {
        word[0] = '\0';
        return(word);
    }

    /*
     * Move char left and skip whitespace
     */

    (*buf)--;

    while (isspace(**buf))
        (*buf)--;

    /*
     * If this char isn't alnum or "_", return it as single word
     */

    if (**buf != '_' && !isalnum(**buf)) {
        word[0] = **buf;
        word[1] = '\0';
        return(word);
    }

    /*
     * Mark current, save terminator, termiante and find beginning of word
     */

    p = *buf + 1;
    temp = *p;
    *p = '\0';

    while (**buf == '_' || isalnum(**buf)) {
        if (*buf == base) {
            (*buf)--;
            break;
        }
        (*buf)--;
    }

    /*
     * *buf is one previous to word, increment, check and save word
     */

    (*buf)++;

    if (strlen(*buf) >= MAXWORD)
        error("word buffer overflow"); 

    strcpy(word,*buf);

    /*
     * Restore char at terminator and return the word
     */

    *p = temp;

    return(word);

}


/*
 * is_typeword  - check if word is a type keyword
 *
 * int is_typeword(char *word);
 *
 * This function returns TRUE if word is a type keyword, FALSE otherwise.
 */

int is_typeword(char *word)
{

    int i;          /* index counter */

    for (i = 0; i < ntwords; i++)
        if (strcmp(twords[i],word) == 0)
            return(TRUE);

    return(FALSE);

}


/*
 * print_words  - print words from string, no break across lines
 *
 * void print_words(int len,int indent,char *string);
 *
 * Prints the words from string with one space between each word, assuming
 * a line length of len characters.  Words will not be broken across lines,
 * and each line starts indent characters from the left.  This indent
 * is included in len.
 */

void print_words(int len,int indent,char *string)
{

    int i;              /* index counter */
    int ce = indent;    /* characters emitted */
    char temp;          /* temp char holder */
    char *p;            /* general purpose char pointer */
    int newline = YES;  /* true if we just started a new line */

    while (*string != '\0') {

        /* skip initial whitespace */

        if (isspace(*string)) {
            string++;
            continue;
        }

        /* find end of word */

        p = string;

        while (!isspace(*p) && *p != '\0')
            p++;

        /* mark it, saving the terminator */

        temp = *p;
        *p = '\0';

        /* add up the length */

        ce += (p - string);

        if (ce == len) {            /* exactly fills line */
            printf("%s",string);        /* so print it */
            newline = YES;              /* say we got a new line */
            ce = indent;                /* restart counter */
            string = NULL;              /* don't want to print it again */
        } else if (ce > len) {      /* overflows line */
            printf("\n");               /* so get a new one */
            newline = YES;              /* say we did */
            ce = indent + (p - string); /* and restart counter */
        }

        if (newline) {  /* need to indent */
            for (i = 0; i < indent; i++)
                putc(' ',stdout);
            newline = NO;
        }

        if (string != NULL) {   /* print if didn't already */

            printf("%s ",string);

            if (++ce == len) {  /* if that got us a new line */
                newline = YES;      /* say so */
                ce = indent;        /* and restart counter */
            }

        }

        string = p;     /* restore string pointer */
        *p = temp;      /* restore char at terminator */

    }

    if (!newline)
        printf("\n");   /* print a newline if didn't get get one */

}


/*
 * usage    - print usage
 *
 * void usage(void);
 *
 * This function prints the usage for this program.
 */

void usage(void)
{
    printf("\n");
    printf("Usage: %s <decl;> [...]\n\n",pname);
    printf("This program prints equivalent English for a C language declaration\n");
    printf("entered on the command line.  The declaration must end with a semi-\n");
    printf("colon.\n\n");
    printf("Direct comments or questions to the author, bixname: wcrowder\n\n");
    printf("Will Crowder\n");
    printf("April 26, 1990\n");
}
