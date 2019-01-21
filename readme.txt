Program: Baker's Dozen DOS, Windows and OS/2 Programs
License: BSD 3-Clause
URL: https://www.os2world.com/wiki/index.php/Baker%27s_Dozen_DOS_and_OS/2_Programs

-------------

This is a set of command line utilities for OS/2 and DOS.

    DELAY is designed to be used in batch (command files) to allow time to read the screen. It takes a single parameter, the number of seconds to wait before continuing.
    WAITIL will wait until the given clock time. It takes a two parameters, the desired hour (on 24 hour clock) and the minute. The user can cause WAITIL to end by pressing the escape key.
    WAITKEY pauses the batch file and waits for the user to press a key exactly as PAUSE does. However, unlike PAUSE, waitkey has a time-out so that the batch file will eventually continue even if the user does not press a key. It takes a one parameter, the number of seconds to wait before timing out (Default = 20 sec).
    LF (ListFiles) will display a directory horizontally, sorted first by extension, and then by file name. It takes multiple parameters to indicate directories and/or wild cards for display. The default is all files in the current directory.
    NF (NewFiles) will display files in a directory sorted with most recent first. It takes up to two parameters, one to indicate directories and/or wild cards for display, and an integer to indicate the number of files to show. It defaults to the most recent 16 files in the current directory.
    LP (LinePrint) will display output one or more files to a PCL printer. Pages are titled and numbered. Font size is adjusted so that the longest line will fit on letter sized paper. Also the text is centred on the page. There are several parameters available. Use LP /? to see the choices.
    SORT is a direct replacement for the DOS (or OS/2) version of SORT and takes the same parameters. However, it removes the DOS (OS/2) limitation of 64K bytes files. The DOS version can use up the the whole 640K and the OS/2 version will sort files of almost limitless size thanks to its use of virtual memory.
    TALLY is similar to the WC UNIX utility. It counts characters, words, lines and pages for the given file(s). If multiple files are specified, TALLY will also provide a summary (total and average values).
    VOLUME queries and displays the volume label of a disk (or partition). It works for fixed and removeable media and redirected (network) volumes. It will not give a critical error. If there is no disk label or if there is no disk present, it returns "No Label". Its single parameter is the disk letter being queried. The default is the current drive.
    FCREATE simple creates a file with no contents (zero byte length). It requires a file name which may be a fully qualified path. If the file already exists, its contents will be removed so use carefully.
    MAKESTR takes any number of decimal, hexidecimal, and text codes and outputs the string equivalent. Decimal input is specified numerically, such as 65 66 (for AB). Hexidecimal ends with the letter h, such as 41h 42h (for AB). Text is preceeded by the at sign (@) such as @AB (for AB). If the parameter includes a DOS or OS/2 special character, enclose the parameter in quotes. This command is useful with redirection to a file or device. For example,

   MAKESTR 1Bh @(s16H >PRN
   sets a PCL printer to 16 pitch.

    TRIM removes trailing blanks from the end of all lines in a text file. It requires two parameters, the input file name and the output file name.
    ADJTEXT attempts to reformat a text file to fit a given width (number of characters). It will try to respect paragraphs and it will hyphenate words if necessary to best fit the given width. Its parameters are the input file name and desired width. Both will be prompted for if not given. Output is sent to stdout (and may be redirected).
    UPTIME simply displays time of last system boot, current time, and the time elapsed between them. You can dare Windows users to keep their machines running as long.
    QLIST Type the command QLIST to display all print queues in your OS/2 system.
    QUERYQ.EXE QUERYQ takes a single argument, the name of a print queue. It displays a wealth of information about the named queue.
    QPRINT also requires the name of a print queue. It then sends all input (from stdin) to the queue for printing. Naturally the input can be redirected from a file or program. 


By Elipse Limited / Jay Schwartz. 

Special thanks to Jay Schwartz for open sourcing this software. 