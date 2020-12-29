@setlocal
@set DOINST=0
@set TMPPRJ=test_btg
@set TMPSRC=..
@set TMPLOG=bldlog-1.txt
@REM set TMPGEN=Visual Studio 16 2019
@set TMP3RD=D:\Projects\3rdParty.x64
@set WORKSPACE=D:\FG\next3
@set TMPSG=%WORKSPACE%\install\msvc140-64
@set TMPBOOST=%WORKSPACE%\windows-3rd-party

@if NOT EXIST %TMP3RD%\nul (
@echo Can NOT locate %TMP3RD%! *** FIX ME ***
@exit /b 1
)
@if NOT EXIST %TMPBOOST%\nul (
@echo Can NOT locate %TMPBOOST%! *** FIX ME ***
@exit /b 1
)
@if NOT EXIST %TMPSG%\include\simgear\nul (
@echo Can NOT locate %TMPSG%\include\simgear! *** FIX ME ***
@exit /b 1
)


@set TMPOPTS=
@REM set TMPOPTS=%TMPOPTS% -G "%TMPGEN%" -A x64
@set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX:PATH=%TMP3RD%
@set TMPOPTS=%TMPOPTS% -DCMAKE_PREFIX_PATH=%TMPSG%;%TMPBOOST%

@if NOT "%DOINST%x" == "1x" (
@echo Install NOT configured! Set DOINST=1
@REM pause
)

@echo Being build %TMPPRJ% %DATE% %TIME% >%TMPLOG%
@echo Doing: 'cmake -S %TMPSRC% %TMPOPTS%'
@echo Doing: 'cmake -S %TMPSRC% %TMPOPTS%' >>%TMPLOG%
@cmake -S %TMPSRC% %TMPOPTS% >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

@echo Doing: 'cmake --build . --config Debug'
@echo Doing: 'cmake --build . --config Debug' >>%TMPLOG%
@cmake --build . --config Debug >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2

@echo Doing: 'cmake --build . --config Release'
@echo Doing: 'cmake --build . --config Release' >>%TMPLOG%
@cmake --build . --config Release >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3
@REM :DONEREL

@echo Appears successful...
@echo.
@if NOT "%DOINST%x" == "1x" (
@echo Install NOT configured! Set DOINST=1
@goto END
)

@echo Continue with install, to %TMP3RD%?
@ask Only 'y' continues. All else aborts...
@if ERRORLEVEL 2 goto NOASK
@if ERRORLEVEL 1 goto INSTALL
@echo Skipping install at this time...
@goto END

:NOASK
@echo Ask utility NOT found...
:INSTALL
@echo Continue with install? Only Ctrl+C aborts
@pause

@echo Doing: cmake --build . --config Release --target INSTALL
@echo Doing: cmake --build . --config Release --target INSTALL >>%TMPLOG%
@cmake --build . --config Release --target INSTALL >>%TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4
@echo.
fa4 " -- " %TMPLOG%
@echo.
@echo All done...

@goto END

:ERR1
@echo cmake config, gen error
@goto ISERR

:Err2
@echo build debug error
@goto ISERR

:ERR3
@echo build release error
@goto ISERR

:ERR4
@echo install error
@goto ISERR

:ISERR
@echo See %TMPLOG% for details...
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
