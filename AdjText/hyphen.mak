#--- Listing 4 ---------------------- HYPHEN.MAK ---------------
# Make hyphen with the Borland C/C++
#
# The -DDEBUG creates a #define DEBUG in the compiled file.
# In this case, a main module is compiled so that the sub-
# routines form a stand-alone test program. Usage is:
#            hyphen <test_input >output
#            hyphen -- <test_input >output
#
# If any command-line arguments are present, debugging diagnos-
# tics that trace the state machine's operation are included.
#
.c.obj:
        bcc -c -v -DDEBUG $*.c

hyphen.exe:     hyphen.obj except.obj
        bcc -v hyphen.obj except.obj

hyphen.obj:     hyphen.c hyphen.h
except.obj:     except.c hyphen.h