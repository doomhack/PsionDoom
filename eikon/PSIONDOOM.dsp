# Microsoft Developer Studio Project File - Name="PSIONDOOM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PSIONDOOM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PSIONDOOM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PSIONDOOM.mak" CFG="PSIONDOOM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PSIONDOOM - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PSIONDOOM - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PSIONDOOM - Win32 Uni Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PSIONDOOM - Win32 Uni Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PSIONDOOM - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\EPOC32\RELEASE\WINS\DEB"
# PROP Intermediate_Dir "\EPOC32\BUILD\PSIONDOOM\WINSD\DEB"
# PROP Ignore_Export_Lib 0
# ADD CPP /nologo /Zp4 /MDd /W4 /ZI /Od /X /I "\PSIONDOOM\EIKON" /I "\PSIONDOOM\EIKON\LIBTIMIDITY" /I "\EPOC32\INCLUDE" /I "\EPOC32\INCLUDE\LIBC" /D "__SYMBIAN32__" /D "__PSISOFT32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "_DEBUG" /FR /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 \EPOC32\RELEASE\WINS\DEB\EDLL.OBJ \EPOC32\RELEASE\WINS\DEB\euser.lib \EPOC32\RELEASE\WINS\DEB\fbscli.lib \EPOC32\RELEASE\WINS\DEB\ws32.lib \EPOC32\RELEASE\WINS\DEB\estlib.lib msvcrt.lib /nologo /entry:"_E32Dll" /subsystem:windows /dll /debug /machine:IX86 /nodefaultlib /WARN:3
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "PSIONDOOM - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\EPOC32\RELEASE\WINS\REL"
# PROP Intermediate_Dir "\EPOC32\BUILD\PSIONDOOM\WINSD\REL"
# ADD CPP /nologo /Zp4 /MD /W4 /O1 /X /I "\PSIONDOOM\EIKON" /I "\PSIONDOOM\EIKON\LIBTIMIDITY" /I "\EPOC32\INCLUDE" /I "\EPOC32\INCLUDE\LIBC" /D "__SYMBIAN32__" /D "__PSISOFT32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "NDEBUG" /FD /c
# ADD MTL /nologo /mktyplib203 /D /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 \EPOC32\RELEASE\WINS\REL\EDLL.OBJ \EPOC32\RELEASE\WINS\REL\euser.lib \EPOC32\RELEASE\WINS\REL\fbscli.lib \EPOC32\RELEASE\WINS\REL\ws32.lib \EPOC32\RELEASE\WINS\REL\estlib.lib /nologo /entry:"_E32Dll" /subsystem:windows /dll /machine:IX86 /nodefaultlib /WARN:3

!ELSEIF  "$(CFG)" == "PSIONDOOM - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\EPOC32\RELEASE\WINS\UDEB"
# PROP Intermediate_Dir "\EPOC32\BUILD\PSIONDOOM\WINSD\UDEB"
# ADD CPP /nologo /Zp4 /MDd /W4 /ZI /Od /X /I "\PSIONDOOM\EIKON" /I "\PSIONDOOM\EIKON\LIBTIMIDITY" /I "\EPOC32\INCLUDE" /I "\EPOC32\INCLUDE\LIBC" /D "__SYMBIAN32__" /D "__PSISOFT32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "_DEBUG" /D "_UNICODE" /FR /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 \EPOC32\RELEASE\WINS\UDEB\EDLL.OBJ \EPOC32\RELEASE\WINS\UDEB\euser.lib \EPOC32\RELEASE\WINS\UDEB\fbscli.lib \EPOC32\RELEASE\WINS\UDEB\ws32.lib \EPOC32\RELEASE\WINS\UDEB\estlib.lib /nologo /entry:"_E32Dll" /subsystem:windows /dll /debug /machine:IX86 /nodefaultlib /WARN:3
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "PSIONDOOM - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\EPOC32\RELEASE\WINS\UREL"
# PROP Intermediate_Dir "\EPOC32\BUILD\PSIONDOOM\WINSD\UREL"
# ADD CPP /nologo /Zp4 /MD /W4 /O1 /X /I "\PSIONDOOM\EIKON" /I "\PSIONDOOM\EIKON\LIBTIMIDITY" /I "\EPOC32\INCLUDE" /I "\EPOC32\INCLUDE\LIBC" /D "__SYMBIAN32__" /D "__PSISOFT32__" /D "__VC32__" /D "__WINS__" /D "__DLL__" /D "NDEBUG" /D "_UNICODE" /FD /c
# ADD MTL /nologo /mktyplib203 /D /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 \EPOC32\RELEASE\WINS\UREL\EDLL.OBJ \EPOC32\RELEASE\WINS\UREL\euser.lib \EPOC32\RELEASE\WINS\UREL\fbscli.lib \EPOC32\RELEASE\WINS\UREL\ws32.lib \EPOC32\RELEASE\WINS\UREL\estlib.lib /nologo /entry:"_E32Dll" /subsystem:windows /dll /machine:IX86 /nodefaultlib /WARN:3

!ENDIF 

# Begin Target

# Name "PSIONDOOM - Win32 Debug"
# Name "PSIONDOOM - Win32 Release"
# Name "PSIONDOOM - Win32 Uni Debug"
# Name "PSIONDOOM - Win32 Uni Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Am_map.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Audio.cpp
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\common.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_client.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_deh.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_items.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_main.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Doomdef.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Doomstat.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Dstrings.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\F_finale.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\F_wipe.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\G_game.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Hu_lib.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Hu_stuff.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_audio.cpp
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_joy.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_main.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_system.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_system_e32.cpp
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_video.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Info.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\instrum.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Lprintf.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_argv.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_bbox.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_cheat.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_menu.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_misc.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_random.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Md5.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\mix.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Mmus2mid.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\output.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_ceilng.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_checksum.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_doors.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_enemy.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_floor.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_genlin.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_inter.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_lights.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_map.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_maputl.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_mobj.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_plats.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_pspr.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_saveg.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_setup.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_sight.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_spec.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_switch.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_telept.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_tick.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_user.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\playmidi.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Psiondoom.mmp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Psiondoom.uid.cpp
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_bsp.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_data.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_demo.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_draw.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_fps.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_main.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_patch.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_plane.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_segs.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_sky.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_things.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\readmidi.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\resample.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\S_sound.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Sounds.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\St_lib.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\St_stuff.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\stream.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Tables.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\timi_tables.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Libtimidity\timidity.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\V_video.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Version.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\W_memcache.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\W_wad.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Wi_stuff.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Z_bmalloc.c
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Z_zone.c
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Am_map.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Audio.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Common.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Config.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_deh.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_englsh.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_event.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_items.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_main.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_net.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_player.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_think.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\D_ticcmd.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Doomdata.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Doomdef.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Doomstat.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Doomtype.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Dstrings.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\F_finale.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\F_wipe.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\G_game.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Hu_lib.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Hu_stuff.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_joy.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_main.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_network.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_sound.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_system.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_system_e32.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\I_video.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Info.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Instrum.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Lprintf.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_argv.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_bbox.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_cheat.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_fixed.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_menu.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_misc.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_random.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\M_swap.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Md5.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Mix.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Mmus2mid.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Options.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Ospaths.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Output.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_checksum.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_enemy.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_inter.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_map.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_maputl.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_mobj.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_pspr.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_saveg.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_setup.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_spec.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_tick.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\P_user.h
# End Source File
# Begin Source File

SOURCE=.\pcm_to_alaw.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Playmidi.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Protocol.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_bsp.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_data.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_defs.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_demo.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_draw.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_fps.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_main.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_patch.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_plane.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_segs.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_sky.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_state.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\R_things.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Readmidi.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Resample.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\S_sound.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Sounds.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\St_lib.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\St_stuff.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Tables.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Timi_endian.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Timi_tables.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Timidity.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\LIBTIMIDITY\Timidity_internal.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\V_video.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Version.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\W_wad.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Wi_stuff.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Z_bmalloc.h
# End Source File
# Begin Source File

SOURCE=\PSIONDOOM\EIKON\Z_zone.h
# End Source File
# End Group
# Begin Group "Build Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PSIONDOOM.MARM
# End Source File
# Begin Source File

SOURCE=.\PSIONDOOM.MARM5
# End Source File
# End Group
# End Target
# End Project
