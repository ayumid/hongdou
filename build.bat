@ECHO OFF

SET TOP=%CD%

SET MODULE=%1

IF "%MODULE%" == "" (
	@ECHO PARAM ERROR
	@ECHO run such as:
	@ECHO     build.bat sample\1-task
	@ECHO     build.bat sample\1-task clean
	goto END_BAT_FILE
)

MKDIR out\bin
DEL /Q out\bin\*.*

IF "%2" == "clean" (
	@DEL /Q /F out\bin\*
	gnumake -C %MODULE% clean
	goto END_BAT_FILE
)

gnumake -C %MODULE%

@echo off  
echo "copy NEZHAC_CP_CNR_MIFI_TX.bin to ./ "  
echo.  


rename .\out\bin\*.bin cp.bin
move .\out\bin\cp.bin .\download\aboot-tools-2021.02.20-win-x64\images\


:END_BAT_FILE

