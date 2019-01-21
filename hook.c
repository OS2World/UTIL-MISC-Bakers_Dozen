
From:    Peter Fitzsimmons
To:      Mike Shillingford                      Msg #460, Jan-08-95 04:40
Subject: subclassing ALL windows on desktop

I don't know what got into me,  but I spent quite a bit of time writing an
example for you (it was a good exercise,  I have written many 16bit os/2
hooks,  but this was the first 32bit one).  I've had someone bugging me to
write a magazine article;  maybe this will be the topic:-)

-----------------------------------------------------------------------

Here is a sample program for a System Hook.  It waits for new window frames
(WC_FRAME) to be created,  then adds a "close" button to the title bar.  This
may seem like a silly example,  but I am starting to like the idea of having a
way to "single-click" a window closed.

This program uses two types of hooks:  A WinSendMsg Hook and an Input Hook.
The SendMsg Hook is required to intercept the WM_CREATE message,which is sent
to the window directly,  not placed in the message queue (ie: posted) of the
window.  The Input Hook is used to monitor the message queue for a special
WM_USER message that is posted by the SendMsg Hook. This is done because
during the WM_CREATE,  the title bar and system menu don't not exist yet.

System input hooks MUST be in a dll.  They are called in the context of the
program being hooked --- this is very significant detail.  If the hook dll has
any "MULTIPLE" data segments,  and/or an "INITINSTANCE" attribute (both of
which are required to use the C runtime in your dll),these actions will be
carried out for _every_ program that is started. In other words,  the DLL
would be reloaded/reinitialized for every program that the system launches.
If you wanted to open a log file, for example,  you would have to open it
again and again as each program is started.

This example program  (and all the other system hooks I have written) does not
use ANY C runtime functions.  It can, therefore,  be "INITGLOBAL" with
"SINGLE" data;  hence,  it is not reinitialized for each program that is
started.  Furthermore,  it does not cause the system to create a new DGROUP
each time a program is started.  As a nice side effect,  the code is small and
fast (the 16bit version of this dll is only 970 bytes!)

    If the purpose of your hook is too complicated to avoid the C
    runtime,  try to split the program in two --- one simple hook dll
    and one PM program that it posts messages too,  where you can use as
    much CRT as you like.

    Keep in mind that these hooks are called for every message in
    the system -- don't waste any time passing on the messages
    you are not interested in.

Another reason to keep them small is System Hooks are very hard to debug
(since they are run in the context of each program being hooked, not the
program that installs the hook).   It is best to test all of your code in a
normal PM program,  then put it into the hook.dll.

The example dll contains the code to install the hooks itself,  so all that is
required to activate it is to load it (DosLoadModule()),  even from a non-PM
program.  Here's a sample that will load it:



 #include <stdio.h>
 #include <conio.h>
 #define INCL_NOPMAPI
 #define INCL_DOS
 #include <os2.h>


 int main(void)
 {
     APIRET rc;
     HMODULE hm = 0;
     rc = DosLoadModule(NULL, 0, "ghook", &hm);
     if(rc){
         printf("sys%04x: Error loading ghook.dll\n", rc);
         return 1;
     }
     getch();
     /* after a key is pressed the program will end and the hooks
      * will disable themselves
      */
     return 0;
 }



[makefile for IBM Cset ++ 2.x -------------------------------------------]


 all: ghook.dll test.exe

 ghook.obj: ghook.c
     icc /Kb /Ms /Gn /O /Gs /Ssem /Rn /c ghook.c

 init.obj: init.asm
     masm /Mx /Zi init.asm;

 ghook.dll: ghook.obj ghook.def init.obj
     icc /Ge- /Gn /Gs /Ssem /Rn ghook.obj ghook.def init.obj os2386.lib

 test.exe: test.c
     icc /Kb /O /Ssem test.c



[init.asm -- masm 5 -----------------------------------------------------]


     .386

 _TEXT segment dword public use32 'CODE'
     assume  cs:_TEXT

 extrn DllInit:PROC

 Init proc near
     jmp DllInit
 Init endp

 _TEXT ends

 end Init



[ghook.c ----------------------------------------------------------------]


 #define INCL_WIN
 #define INCL_DOS
 #include <os2.h>

 static HAB ghab = 0;
 static int ghm = 0;

 BOOL EXPENTRY InputHook(HAB hab, PQMSG pQmsg, USHORT fs)
 {

     if(pQmsg->msg == WM_USER + 0x848){ /* this user message should really */
                                        /* be an atom! */
         HWND hwndSysMenu;
         MENUITEM mi;
         HWND hwnd = pQmsg->hwnd;

         hwndSysMenu = WinWindowFromID(hwnd, FID_SYSMENU);
         if(!hwndSysMenu)
             return 0;
         mi.iPosition   = MIT_END;
         mi.afStyle     = MIS_TEXT | MIS_SYSCOMMAND;
         mi.afAttribute = 0;
         mi.id          = SC_CLOSE;
         mi.hwndSubMenu = 0;
         mi.hItem       = 0;

         WinSendMsg(hwndSysMenu, MM_INSERTITEM, (MPARAM)&mi, "Close");

         WinSendMsg(hwnd, WM_UPDATEFRAME, (MPARAM)FCF_SYSMENU, NULL);
         DosBeep(2000, 50);  /* this let's us know it is working -- remove. */
         return TRUE;        /* do not pass to next hook proc */
     }
     return 0;
 }

 VOID EXPENTRY SendMsgHook(HAB hab, PSMHSTRUCT psmh, BOOL fInterTask)
 {
     PCREATESTRUCT pCrs;
     PFRAMECDATA   fcd;

     if(psmh->msg != WM_CREATE)
         return;     /* not creating a window */

     pCrs =  (PCREATESTRUCT)psmh->mp2;
     if(pCrs->pszClass != WC_FRAME)
         return;     /* not a frame. */

     fcd = (PFRAMECDATA)psmh->mp1;
     if(!fcd || !(fcd->flCreateFlags & FCF_TITLEBAR))
         return;     /* no title bar */

     if(!(fcd->flCreateFlags & FCF_SYSMENU))
         return;     /* no sys menu */

     /* Post a message so that it is received (above in the input hook) */
     /* after the whole frame has been created. */
     WinPostMsg(psmh->hwnd, WM_USER + 0x848, 0, 0);
 }

 void APIENTRY CleanUp(void)
 {
     WinReleaseHook(ghab, NULLHANDLE, HK_SENDMSG, (PFN)SendMsgHook, ghm);
     WinReleaseHook(ghab, NULLHANDLE, HK_INPUT, (PFN)InputHook, ghm);
     DosExitList(EXLST_EXIT, NULL);
 }

 static int Main(void)
 {
     if( !(ghab = WinInitialize(0)) )
         return FALSE;

     if(!WinSetHook(ghab, NULLHANDLE, HK_SENDMSG, (PFN)SendMsgHook, ghm))
         return FALSE;

     if(!WinSetHook(ghab, NULLHANDLE, HK_INPUT, (PFN)InputHook, ghm))
         return FALSE;

     return TRUE;
 }

 int DllInit(unsigned hmod, unsigned termflag )
 {
     if(termflag == 0){
         ghm = hmod;
         DosExitList(EXLST_ADD, (PFNEXITLIST)CleanUp);
         DosBeep(4000, 200);
         return Main();
     }
     return 1;
 }



[ghook.def -------------------------------------------------------------]


 LIBRARY GHOOK INITGLOBAL

 DATA SINGLE

 EXPORTS
     DllInit     @1



[test.c ----------------------------------------------------------------]

 #include <stdio.h>
 #include <stdlib.h>
 #include <conio.h>
 #define INCL_NOPMAPI
 #define INCL_DOS
 #include <os2.h>
 int main(void)
 {
     APIRET rc = 0;
     HMODULE hm = 0;
     rc = DosLoadModule(NULL, 0, "ghook", &hm);
     if(rc){
         printf("sys%04x: Error loading ghook.dll\n", rc);
         exit(rc);
     }
    /* after a key is pressed the program will end and the hooks
     * will disable themselves
     */
     getch();
 }


