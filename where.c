/*

    Whereos2  File finder for OS/2

Written by:   Bob Whitten, P.O. Box DD, Tujunga, CA 91042
    bix name:  bwhit;	   Compuserve ID: 72220,2137

The code is hereby released to the public domain.  It is offered AS-IS,
without any assurance that it will do anything useful or that it will
not do something harmful.
*/

#define INCL_BASE
#include <os2.h>
#include <stdio.h>


void dirlook(char *s);

#ifdef INCL_BASE
unsigned long disknum;
unsigned long far logdrv;
unsigned long far pathlen =256;
unsigned char far path[257];
#endif

char dirtxt[133];

void main(int argc, char *argv[])
    {
 int ErrorCode;
int i, dotflag;
   char ch, look4[15];

    if (argc != 2)
    {
    printf("whereos2 Usage: whereos2 <filename pattern>\n"
        "   <filename pattern> may contain wildcards.\n"
        "   If no '.' is given, one will be assigned, and will search\n"
        "    for all extensions.\n\n"
        "Brought to you by:  Bob Whitten, P.O. Box DD, Tujunga, CA 91042\n");
    exit(0);
    }
    dotflag = 0;
    for (i=0; i < 14; i++)
    {
    ch = argv[1][i];
    look4[i] = ch;
    if (ch == '.')
        dotflag = 1;
    }
    if ( ! dotflag)
    strcat(look4, ".*");
    printf("Searching for pattern: for %s \n\n", look4);

#ifdef INCL_BASE
    ErrorCode = DosQCurDisk(&disknum, &logdrv);
    if (ErrorCode) DosExit(1, ErrorCode);
    ErrorCode = DosQCurDir(0, path, &pathlen);	  /* save this for later */
    if (ErrorCode) DosExit(1, ErrorCode);
#endif
    dirtxt[0] = '\\';	    /* simply, the root */
    dirtxt[1] = '\0';
    ErrorCode = chdir( dirtxt );	    /* change to the root */
    if (ErrorCode) DosExit(1, ErrorCode);

    dirlook(look4);

    ErrorCode = chdir( path );
    if (ErrorCode) DosExit(1, ErrorCode);
    }

char mons[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

void dirlook(char *fname)
    {
    int i;
    unsigned int ErrorCode;
    FILEFINDBUF ffbuf;
    unsigned long searchndl = 0xffffffff;
    unsigned int attr = 0x0037; 	/* normal and dir files */
    unsigned long searchcnt = 1; /* just get 1, */
    int savdirtxtl;

 /*   FIRST LOOK FOR "fname" MATCHES */

    ErrorCode = DosFindFirst( fname, &searchndl, attr,
        &ffbuf, sizeof(ffbuf),
        &searchcnt, 0L);
    if (ErrorCode == ERROR_NO_MORE_FILES)
    goto SEARCHDIR; 	/* a "goto" is sometimes considered
                   bad form, but it seemed appropriate here */
    else if (ErrorCode != 0)
     DosExit(1, ErrorCode);
    while (searchcnt)
    {
    if (ffbuf.attrFile & 0x10)
        printf("%s%s  <DIR>\n", dirtxt, ffbuf.achName);
    else
        printf("%s%s   %ld  %d-%3s-%02d \n",
           dirtxt, ffbuf.achName, ffbuf.cbFile,
           ffbuf.fdateLastWrite.day, mons[ffbuf.fdateLastWrite.month],
           (ffbuf.fdateLastWrite.year+80) % 100);
    ErrorCode = DosFindNext(searchndl, &ffbuf, sizeof(ffbuf), &searchcnt);
    if ((ErrorCode != ERROR_NO_MORE_FILES) && (ErrorCode != 0))
         DosExit(1, ErrorCode);
    }
    ErrorCode = DosFindClose(searchndl);
    if (ErrorCode != 0)
    DosExit(1, ErrorCode);


/*  NOW LOOK FOR DIRECTORIES, (except "." and "..")  */

SEARCHDIR:
    searchcnt = 1;
    searchndl = -1;
    ErrorCode = DosFindFirst( "*.*", &searchndl, attr,
        &ffbuf, sizeof(ffbuf),
        &searchcnt, 0L);
    if (ErrorCode == ERROR_NO_MORE_FILES)
    return;
    else if (ErrorCode != 0)
     DosExit(1, ErrorCode);
    while (searchcnt)
    {
    if ((ffbuf.attrFile & 0x10) && (ffbuf.achName[0] != '.'))      /* check for directory */
        {
        ErrorCode = chdir( ffbuf.achName );
        if (ErrorCode)
        DosExit(1, ErrorCode);
        savdirtxtl = strlen(dirtxt);
        strcat(dirtxt, ffbuf.achName);
        strcat(dirtxt, "\\");
        dirlook(fname);	    /* go for it, recursively */
        dirtxt[savdirtxtl] = '\0';
        ErrorCode = chdir( ".." );
        }
    ErrorCode = DosFindNext(searchndl, &ffbuf, sizeof(ffbuf), &searchcnt);
    if ((ErrorCode != ERROR_NO_MORE_FILES) && (ErrorCode != 0))
         DosExit(1, ErrorCode);
    }
    ErrorCode = DosFindClose(searchndl);
    if (ErrorCode != 0)
    DosExit(1, ErrorCode);

    }


