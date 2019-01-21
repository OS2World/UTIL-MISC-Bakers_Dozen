/* HIDE.C  ----- 1996 May 06 */

/* From: Michael W Koehler 76636,1207 (CIS) */

/* Routine to hide a VIO (text) window */

/*
 Simple enough.  Is used this specifically for a rexx script that started
 processes but which I wanted to be detached.  Since that wasn't possible,
 hiding it like this did the job.  Any VIO style process can be hiden this way.
 This has been tested on Warp Fullpack with Fixpacks 10 and 17.
*/

    /*  Get our window handle */
    HWND ourWindow = getOurHWnd( );
    if (ourWindow == 0)
        {
        printf( "\nUnable to find this window.\n" )
        return 1;
        }

    /*  Hide the window */
    rc = WinSetWindowPos( ourWindow, NULL, 0, 0, 0, 0, SWP_HIDE );

    /*  Get our switch control block */
    HSWITCH hSwitch = WinQuerySwitchHandle( ourWindow, 0 );
    SWCNTRL switchControl;
    WinQuerySwitchEntry( hSwitch, &switchControl );

    /*  Hide our switch list entry */
    switchControl.uchVisibility = SWL_INVISIBLE;
    WinChangeSwitchEntry( hSwitch, &switchControl );


/* The trick is getting the HWND ourWindow.  We do this by searching the switch list... */


/*  Find our hwnd */
HWND getOurHWnd( void )
    {
    HAB      hab        = NULLHANDLE;
    ULONG    numItems   = 0;            /* Number of items in list */
    ULONG    bufferSize = 0;            /* Size of buffer for information */
    PSWBLOCK pswblk     = NULL;         /* Pointer to information returned */
    PPIB     pib;
    PTIB     tib;
    HWND     ourWindow    = 0;
    int      i;

    /*  Get our process id */
    if (DosGetInfoBlocks( &tib, &pib ) != NO_ERROR)
        return 0;                       /* HWND 0 means we couldn't get it */

    /*  Get the number of items in switch list */
    numItems = WinQuerySwitchList( hab, NULL, 0 );

    /*  Calculate size of buffer needed and allocate memory */
    bufferSize = (numItems * sizeof( SWENTRY )) + sizeof( HSWITCH );
    pswblk = (PSWBLOCK) malloc( bufferSize );

    /*  Get switch list info */
    WinQuerySwitchList( hab, pswblk, bufferSize );

    /*  Search switch list for our ourParent's pid and return matching hwnd */
    for (i = 0;  i < pswblk->cswentry;  i++)
        {
        if (pswblk->aswentry[i].swctl.idProcess == pib->pib_ulppid)
            {
            ourWindow = pswblk->aswentry[i].swctl.hwnd;
            break;
            }
        }

    /*  Done */
    free( pswblk );
    return ourWindow;
    }

/*
 Unhiding is a simple variation.  Just reverse the two steps for hiding.
 I have not been able to get Minimize to go, even though it should be 
 another simple variant.
 Anybody out there who knows why hide works but minimize does not?
*/

