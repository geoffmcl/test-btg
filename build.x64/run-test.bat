@setlocal
@set TMPEXE=Release\test-btg.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMP3RD=X:\3rdParty.x64\bin
@if NOT EXIST %TMP3RD%\nul goto NOBIN

@set TMPCMD=
:RPT
@if "%~1x" == "x" goto GOTCMD
@set TMPCMD=%TMPCMD% %1
@shift
@goto RPT
:GOTCMD

@REM No longer needed - using static libs
@REM But if using the DLL, then
@set PATH=%TMP3RD%;%PATH%

%TMPEXE% %TMPCMD%

@goto END

:NOEXE
@echo Error: Can NOT locate %TMPEXE%! *** FIX ME *
@goto END

:NOBIN
@echo Error: Can NOT locate %TMPBIN%! *** FIX ME *
@goto END

:END
