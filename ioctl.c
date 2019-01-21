
From:    Peter Fitzsimmons
To:      Bill Shymanski                         Msg #488, Jan-13-95 00:51
Subject: OS/2 1.1 vs. 2.1 API

 BS> goes out the window in 2.X world.  But how come DosDevIIOCtl went from 5
 BS> parameters to 9 ?

Good question -- I have yet to see an ioctl call that could not have been done
the old way.  Anyway,  here's some stupid no-nothing source that successfully
calls an ioctl (once you have seen it done once,it all falls into place):


 /* This sample will help you to the point of getting the ioctl's working.
 Beginner's have a lot of problems with them.   A good comm program,
 unlike this one,  would have a dedicated read thread,  blocked on
 DosRead().  A good program may even have a write thread too. */



  #define INCL_NOPMAPI          /* no PM in this program */
  #define INCL_DOS
  #define INCL_DOSDEVIOCTL      /*DosDevIOCtl Values */
  #include <os2.h>
  #include <stdio.h>


  int main(void)
  {
      HFILE hf;
      APIRET rc;
      ULONG ulAction;
      ULONG bps;
      ULONG ulpinout, uldinout;
      DCBINFO dcb;

      /* Always open the modem in OPEN_SHARE_DENYREADWRITE!!!  Pass the open
       * handle to child programs (doors) that also want to use the modem.
       */
      rc = DosOpen("COM1",
                      &hf,
                      &ulAction,
                      0UL, FILE_NORMAL,
                      OPEN_ACTION_OPEN_IF_EXISTS,
                      OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE, 0);
      if(rc){
          printf("sys%04u: dosopen(comx)\n", rc);
          return 1;
      }

       /* set baud rate */
      ulpinout = sizeof(bps);
      bps = 9600;
      rc  = DosDevIOCtl(  hf, IOCTL_ASYNC, ASYNC_SETBAUDRATE, &bps,
                          ulpinout, &ulpinout, NULL, 0, NULL );
      if(rc){
          printf("sys%04u: dosioctl(setbaud)\n", rc);
          return 1;
      }

      /* For future compatabilty, always read the current DCB and change
       * only those bits you are insterested in.  NEVER stuff your own
       * values into a setdcb call.
       */

      ulpinout=uldinout=0;
      rc = DosDevIOCtl(hf, IOCTL_ASYNC, ASYNC_GETDCBINFO,
              NULL, 0, &ulpinout, &dcb, sizeof(dcb), &uldinout);
      if(rc){
          printf("sys%04u: dosioctl(getdcb)\n", rc);
          return 1;
      }

     dcb.fbTimeout &= ~(MODE_WAIT_READ_TIMEOUT | MODE_READ_TIMEOUT); /* turn off */
     dcb.fbTimeout |= MODE_NOWAIT_READ_TIMEOUT;                      /* turn on */

      ulpinout=uldinout=0;
      rc = DosDevIOCtl(hf, IOCTL_ASYNC, ASYNC_SETDCBINFO,
              &dcb, sizeof(dcb), &ulpinout, NULL, 0, &uldinout);

      if(rc){
          printf("sys%04u: dosioctl(setdcb)\n", rc);
          return 1;
      }

      printf("ok\n");
      return 0;
  }


