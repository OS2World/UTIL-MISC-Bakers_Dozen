# /* $title:     'MAKEFILE ========== Generic Makefile ==========' */
# /* $subtitle:  'Elipse Ltd.  [jms]         Revised: 1997 Oct 07' */
#

# For debug version, define Debug = 1. For Optimized version rem it out!

# Debug =     1

!IFDEF Debug
CompOpt =   /Tm+i+x+ /O- /C /Fl$(TMP)icc.lst /L+s+p58i- /G4m+e+f+d-h+ /Q /Wall+gen-ppt-uni-trd-ext-ppc- /W3 /Re /V"NotOptim" /DDEBUG=1
LinkOpt =   /NoFreeFormat /Base:0x10000 /ExePack /NoE /Align:4 /PM:vio /Debug /Map
ExtraObj =  \IbmCpp\lib\SetArgV.Obj \IbmCpp\lib\cppopa3.obj ### \DevTools\PMPrintf\printf.obj 
AllLib =    Elipse GpfP2CIS ErrorMgr
!ELSE
CompOpt =   /Tm-i-x- /O+ /C /Fl$(TMP)icc.lst /L+s+p58i- /G4m+e+f+d-h- /Q /Wall+gen-ppt-uni-trd-ext- /W3 /Re /V"Elipse Ltd."
LinkOpt =   /NoFreeFormat /Base:0x10000 /ExePack /NoE /Align:4 /PM:vio
ExtraObj =  \IbmCpp\lib\SetArgV.Obj
AllLib =    Elipse GpfP2CIS
!ENDIF

.SUFFIXES:      .Orc  .C  .Ipf  .Hlp  .Rc  .Res  .Obj  .Exe

.Obj.Exe:       # Rule to link progs
                echo $** | MakeResp.Exe      >$(TMP)icc.rsp
                echo $(ExtraObj)            >>$(TMP)icc.rsp
                echo $*.exe                 >>$(TMP)icc.rsp
                echo $*.map                 >>$(TMP)icc.rsp
                echo $(AllLib)              >>$(TMP)icc.rsp
                if exist $*.def echo $*.def >>$(TMP)icc.rsp
                echo.                       >>$(TMP)icc.rsp
                iLink.Exe $(LinkOpt) @$(TMP)Icc.Rsp
                if exist $*.res      rc  $*.res
                if exist $*.cmd      del $*.cmd
                if exist $*.mak      del $*.mak
                if exist $*.l        del $*.l
                echo.
                echo            $*.exe Built and Ready!

.Ipf.Hlp:       # Rule to build help
                ipfc $*.ipf

.Rc.Res:        # Rule to build resource
                rc -r $*.rc

.C.Obj:         # Rule to compile
                icc.exe $(CompOpt) $*.c

.Orc.C:         # Rule to generate source
                if exist $(TMP)*.lst del $(TMP)*.lst
                if exist $(TMP)*.log del $(TMP)*.log
                GpfGener $*.orc
                touch.exe $*.c $*.ipf $*.rc

.Orc.Ipf:       # Rule to generate help source
                GpfGener $*.Orc
                touch.exe $*.c $*.ipf $*.rc

.Orc.Rc:        # Rule to generate resource source
                GpfGener $*.orc
                touch.exe $*.c $*.ipf $*.rc


Project:        crc.exe

crc.exe:        crc.obj
crc.obj:        crc.c
