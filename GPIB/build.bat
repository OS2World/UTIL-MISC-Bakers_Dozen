@echo off
rem             Build

echo.
echo Build interface to NI GPIB DLL
echo and a test program to try out.
echo.

pause

if exist gpib_c.obj del gpib_c.obj
if exist gpib_test.obj del gpib_test.obj
if exist gpib_test.exe del gpib_test.exe
if exist wildargv.obj del wildargv.obj

wcl386 gpib_test.c gpib_c.c

echo.
echo Done
echo.
