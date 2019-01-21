
From:    Peter Fitzsimmons                      Rec'd
To:      Sim Brigden                            Msg #481, Jan-12-95 00:50
Subject: Closing an active program

 SB> I'm no programmer.  How do I close an active program
 SB> from the command line.  I am creating a special

The following program uses documented OS/2 calls to allow you to kill a task
by name.  The caveat is that the program must appear in the desktop "Window
List".

If you want to kill a program not listed in the window list,  there is an
undocumented API (which has changed at least once already) which will allow
this -- let me know.

If you don't have a 16bit C compiler kicking around,  you can download the
compiled program as KT.LZH from the number shown below.



 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #define INCL_WIN
 #define INCL_WINSWITCHLIST
 #define INCL_DOSPROCESS
 #include <os2.h>

 /*
  * KT.C (kill session)
  *
  * This program kills a session by name,  or lists the titles of
  * all currently running sessions.
  *
  * Usage: KT
  *        lists current tasks.
  *
  * Usage: KT <task string>
  *        Kills the task that has <task string> anywhere in its name.
  *
  *
  * MSC 6.00 (5.10 prob ok),  memory model indep.
  *
  * P. Fitzsimmons. 01:20:05 Mon  07-02-1990
  */

 void cdecl main(int argc, char **argv)
 {
     USHORT num, i;
     SWBLOCK *pSwb;
     SWENTRY *swFound = NULL;

     if (argc > 1)
         strupr(argv[1]);
     num = WinQuerySwitchList(0L, NULL, 0);
     if (!num) {
         puts("Unexpected error");
         return;
     }
     pSwb = malloc(sizeof(USHORT) + sizeof(SWENTRY) * num);
     WinQuerySwitchList(0L, pSwb, sizeof(USHORT) + sizeof(SWENTRY) * num);
     if (argc == 1)
         printf("PID  Task Title\n---------------\n");
     for (i = 0; i < num; i++) {
         if (argc == 1)
             printf("%04x %s\n", pSwb->aswentry[i].swctl.idProcess,
                                 pSwb->aswentry[i].swctl.szSwtitle);
         else if (strstr(strupr(pSwb->aswentry[i].swctl.szSwtitle), argv[1])) {
             printf("Found '%s'\n", pSwb->aswentry[i].swctl.szSwtitle);
             if (swFound) {
              printf("There is more than one task matching '%s' -- aborting\n",
                         argv[1]);
                 exit(1);
             }
             swFound = &pSwb->aswentry[i];
         }
     }
     if (swFound) {
         printf("Killing %s\n", swFound->swctl.szSwtitle);
         WinSwitchToProgram(swFound->hswitch);
         DosKillProcess(DKP_PROCESS, swFound->swctl.idProcess);
     }
     free(pSwb);
     if (argc == 1) {
         printf("\nTo KILL a program,  type '%s <task name>'\n", argv[0]);
     }
     else if (!swFound) {
         printf("'%s' could not be found in any of the task titles!\n",
            argv[1]);
     }
 }



Note that this will list all of your open WPS folders -- if you kill any of
them it will actually kill the WPS (which will restart though).


