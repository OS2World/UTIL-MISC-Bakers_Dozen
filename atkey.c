/* title:       'ATKEY.C ========= Turn NUMLOCK on =========' */
/* subtitle:    'Elipse Ltd.  [jms]     Revised: 1992 Jul 22' */


#define INCL_SUB

#include <os2def.h>
#include <bsesub.h>
#include <stdio.h>


int main(void)
    {
    KBDINFO kbdstatus;
    
    kbdstatus.cb = sizeof(kbdstatus);
    KbdGetStatus(&kbdstatus, 0);

    if ((kbdstatus.fsState & 0x0020) == 0)
        {
        kbdstatus.fsMask = 0x0010;      /* fsState is to be modified */
        kbdstatus.fsState |= 0x0020;    /* set numlock bit on */
        KbdSetStatus(&kbdstatus, 0);    /* set new state */
        }
    return 0;
    }
