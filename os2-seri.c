X-NEWS: lu comp.os.os2.programmer: 525
Path: luga!ucsvc.ucs.unimelb.edu.au!ariel!munnari.oz.au!samsung!dali.cs.montana.edu!rpi!usc!wupost!waikato.ac.nz!comp.vuw.ac.nz!am.dsir.govt.nz!marcamd!mercury!kcbbs!kc
Newsgroups: comp.os.os2.programmer
Subject: Problem With OS/2 Serial Line Program
Message-ID: <1991Jul24.023603.12789@kcbbs.gen.nz>
From: Chris_Sullivan@kcbbs.gen.nz (Chris Sullivan)
Date: 24 Jul 91 02:36:03 GMT
References: <1991Jul18.140638.24515@cse.uta.edu>
Organisation: Kappa Crucis Unix BBS, Auckland, New Zealand
Lines: 230

Here's a program that works.  Hope it's helpfull to you.


/************************************************************************/

/*  TERM.CPP  -- from "PC Magazine" October 17, 1989 pg 285-295		*/

/*  Chris Sullivan  5 June 1991						*/

/*  Zortech C++ 2.1							*/

/************************************************************************/



#define INCL_DOS

#define INCL_SUB

#define INCL_DOSDEVICES



#include <os2.h>

#include <stdio.h>

#include <stdlib.h>



#define SERIAL		0x01

#define SETBAUDRATE	0x41

#define SETLINECTRL	0x42

#define SETDCBINFO	0x53

#define GETDCBINFO	0x73



#define STACK_SIZE	2048

#define BPS		4800

#define KBD_HANDLE	0

#define VIO_HANDLE	0



struct {

  unsigned char dataBits;

  unsigned char parity;

  unsigned char stopBits;

  }  lineCtrl = { 8,		/*8 data bits */

		  0,		/*No parity */

		  0 };		/*1 stop bits */



struct {

  unsigned short usWriteTimeOut;

  unsigned short usReadTimeOut;

  unsigned char  bFlags1;

  unsigned char  bFlags2;

  unsigned char  bFlags3;

  unsigned char  bErrorReplacementChar;

  unsigned char  bBreakReplacementChar;

  unsigned char  bXONChar;

  unsigned char  bXOFFChar;

  } dcbInfo;



HFILE hCom;			/*COM handle */

unsigned char inBuffer[256];	/*input buffer */



void far ComThread(void);





/************************************************************************/

/*  main								*/

/************************************************************************/

void main(void) {

  USHORT		usAction, rc;

  USHORT		usBaudRate = BPS;

  unsigned		threadID;

  static char		comThdStk[STACK_SIZE];

  struct _KBDKEYINFO	kbdData;

  USHORT		usBytesWritten;



/*Open and initialize COM1 */

  if (DosOpen((unsigned char far *)"COM1", &hCom, &usAction, 

	0L, 0, 1, 0x12, 0L)) {

    printf("COM1 not available or COM0x.SYS not loaded\n");

    exit(1);

    }



/*Set data rate to 4800bps and line format to N81 */

  rc= DosDevIOCtl(0L, &usBaudRate, SETBAUDRATE, SERIAL, hCom);

  rc= DosDevIOCtl(0L, &lineCtrl,   SETLINECTRL, SERIAL, hCom);



/*Set device control block parameters */

  rc= DosDevIOCtl(&dcbInfo,    0L, GETDCBINFO, SERIAL, hCom);

  dcbInfo.usWriteTimeOut = 6000;		/*60 second write timeout */

  dcbInfo.usReadTimeOut  = 6000;		/*60 second readtimeout */

  dcbInfo.bFlags1 = 0x01;			/*enable DTR */

  dcbInfo.bFlags2 = 0x40;			/*enable RTS */

  dcbInfo.bFlags3 = 0x04;			/*wait-for-something reads */

  rc= DosDevIOCtl(0L, &dcbInfo,    SETDCBINFO, SERIAL, hCom);



/*Create a thread to monitor the serial port */

  rc= DosCreateThread((PFNTHREAD)ComThread, (PTID)&threadID,

	(PBYTE)comThdStk+STACK_SIZE);



/*Monitor the keyboard and output typed characters */

  do {

    KbdCharIn(&kbdData, IO_WAIT, KBD_HANDLE);

    if ((kbdData.chChar != 0) && (kbdData.chChar != 0x1B)) {

      VioWrtTTY(&kbdData.chChar, 1, VIO_HANDLE);

      DosWrite(hCom, &kbdData.chChar, 1, &usBytesWritten);

      }

    } while (kbdData.chChar != 0x1B);	/*stop on ESCAPE character */



  DosExit(EXIT_PROCESS,0);

}





/************************************************************************/

/*  Thread to read characters from COM1 and write to screen		*/

/************************************************************************/

#pragma check_stack(off)	/*disable stack checking */



void far ComThread(void) {  

  unsigned short usBytesRead, i;



  while (!DosRead(hCom, inBuffer, 256, &usBytesRead))

    if (usBytesRead) {

      for (i=0; i<usBytesRead; i++)  inBuffer[i] &= 0x7f;

      VioWrtTTY(inBuffer, usBytesRead, VIO_HANDLE);

      }

  DosExit(EXIT_THREAD, 1);

}
