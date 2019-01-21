/****************************************************************************

From:    Peter Fitzsimmons                      Sent
To:      Martin Maney                           Msg #1052, Aug-30-92 00:19:52
Subject: PostScript cart. in Laserjet, or, where did ^D go?

 MM> It's a long story, and there might well be solutions
 MM> that aren't entirely on-topic here (but I'm desparate

This IS off-topic;  so I'll answer it with a programmer's solution to nudge it
back on topic.

Here's a little gem they Gerry wrote.   Let's pretend your printer is really
on LPT1.  You would start up prmon.exe like this:

  PRMON LPT2 LPT1 header.doc trailer.doc

and get your applications to print to LPT2.  Prmon will prepend "header.doc"
and append "trailer.doc" to each print job.  This is great for dual-duty
printers,  such as my NEC Silentwriter/2,  wich require special print codes to
switch them bewteen PostScript and HP emulation.

Gerry originally wrote it to have a separate "print tiny" and "print normal"
ports on his network file server.

****************************************************************************/


 /* prmon.c */

 #define INCL_BASE
 #include <stdlib.h>
 #include <stdio.h>
 #include <os2.h>

 #define BUFSIZE 1024

 typedef struct _mymonin {
     USHORT cb;
     BYTE reserve[18];
     BYTE buffer[BUFSIZE];
 } MYMONIN;

 HMONITOR monitor;
 MONIN Inbuf;
 MYMONIN Outbuf;

 BYTE mybuf[4096];
 USHORT LastSpool = 0;
 BYTE filebuf[16384];

 int main(int argc, char **argv)
 {
     register int x;
     USHORT rc, Size;
     FILE *SpoolFile = NULL;
     FILE *Append;
     USHORT usAction;

     if (argc != 5) {
         printf("Useage is:- PrMon DevMonitor DevDest HeaderFile
TrailerFile\n");
         exit(-1);
     }

     /* Open the printer monitor  */

     Inbuf.cb = sizeof(MYMONIN);
     Outbuf.cb = sizeof(MYMONIN);

     rc = DosMonOpen(argv[1], &monitor);
     if (rc != 0) {
         printf("Error Opening the Monitor\n");
         exit(-1);
     }

     /* Go Time Critical  */
     DosSetPrty(PRTYS_PROCESS, PRTYC_TIMECRITICAL, 0, 0);

     rc = DosMonReg((HMONITOR) monitor, (PBYTE) & Inbuf, (PBYTE) & Outbuf,
MONITOR_END, 1);
     if (rc != 0) {
         printf("Error %d Registering the Monitor\n", rc);
         exit(-1);
     }

     /* Process the packets  */
     while (1 == 1) {
         Size = 4096;
         for (x = 0; x < BUFSIZE; x++)
             mybuf[x] = 0;
         DosMonRead((PBYTE) & Inbuf, DCWW_WAIT, mybuf, &Size);
         switch (mybuf[0]) {
             case 1:                     /* open the print job  */
                 SpoolFile = fopen(argv[2], "wb");
                 printf("Starting Job %d\n", Size);
                 /* Open the header file  */
                 Append = fopen(argv[3], "rb");
                 /* Read it in  */
                 rc = fread(filebuf, 1, 16384, Append);
                 /* write it to the spooler  */
                 fwrite(filebuf, rc, 1, SpoolFile);
                 fclose(Append);

                 break;

             case 2:                     /* close and print job  */
                 /* print the thing on the sun  */
                 printf("Ending Job %d\n", Size);
                 Append = fopen(argv[4], "rb");
                 rc = fread(filebuf, 1, 16384, Append);
                 fwrite(filebuf, 1, rc, SpoolFile);
                 fclose(Append);
                 fclose(SpoolFile);
                 SpoolFile = NULL;
                 break;

             default:                    /* write data to the file  */
                 x = mybuf[0];
                 fwrite(&mybuf[4], 1, Size - 4, SpoolFile);
                 break;
         }
     }
 }

