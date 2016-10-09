@setlocal
@set TMPPRJ=test-btg
@echo Build %TMPPRJ% project, in 64-bits
@set TMPLOG=bldlog-1.txt
@set BLDDIR=%CD%
@set TMPVERS=
@set TMPSRC=..
@set VCVERS=14
@set SET_BAT=%ProgramFiles(x86)%\Microsoft Visual Studio %VCVERS%.0\VC\vcvarsall.bat
@if NOT EXIST "%SET_BAT%" goto NOBAT
@set TMPOPTS=-G "Visual Studio %VCVERS% Win64"
@set DOPAUSE=pause

@REM To find OSG, and 3rdParty headers, libraries
@set TMPSG=X:\install\msvc%VCVERS%0-64\simgear
@set TMP3RD=X:\3rdParty.x64
@set TMPBOOST=c:\local\boost_1_62_0
@REM set TMPOSG=%TMPDRV%\install\msvc100-64\OpenSceneGraph
@REM set TMPBOOST=X:\install\msvc%VCVERS%0-64\boost
@if NOT EXIST %TMP3RD%\nul goto NO3RD
@if NOT EXIST %TMPSG%\nul goto NOSG
@if NOT EXIST %TMPBOOST%\nul goto NOBOOST

:RPT
@if "%~1x" == "x" goto DNCMD
@if "%~1x" == "NOPAUSEx" (
@set DOPAUSE=echo No pause requested
) else (
@echo Only one command NOPAUSE allowed
@goto ISERR
)
@shift
@goto RPT
:DNCMD

@call chkmsvc test_btg

@echo Begin %DATE% %TIME%, output to %TMPLOG%
@echo Begin %DATE% %TIME% > %TMPLOG%

@REM ############################################
@REM NOTE: SPECIAL INSTALL LOCATION
@REM Adjust to suit your environment
@REM ##########################################
@if /I "%PROCESSOR_ARCHITECTURE%" EQU "AMD64" (
@set TMPINST=%TMP3RD%
) ELSE (
 @if /I "%PROCESSOR_ARCHITECTURE%" EQU "x86_64" (
@set TMPINST=%TMP3RD%
 ) ELSE (
@echo ERROR: Appears 64-bit is NOT available - *** FIX ME ***
@goto ISERR
 )
)

@echo Doing: 'call "%SET_BAT%" %PROCESSOR_ARCHITECTURE%'
@echo Doing: 'call "%SET_BAT%" %PROCESSOR_ARCHITECTURE%' >> %TMPLOG%
@call "%SET_BAT%" %PROCESSOR_ARCHITECTURE% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR0

@REM call setupqt64
@cd %BLDDIR%

:DNARCH

@set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX=%TMPINST%
@set TMPOPTS=%TMPOPTS% -DCMAKE_PREFIX_PATH=%TMPSG%;%TMPBOOST%
@REM set TMPOPTS=%TMPOPTS% -DCMAKE_PREFIX_PATH:PATH=%TMP3RD%;%TMPOSG%
@REM set TMPOPTS=%TMPOPTS% -DENABLE_TESTS:BOOL=OFF

@if NOT EXIST %TMPSRC%\nul goto NOSRC
@if EXIST build-cmake.bat (
@call build-cmake >> %TMPLOG%
)
@if NOT EXIST %TMPSRC%\CMakeLists.txt goto NOCM

@REM echo. >> %TMPLOG%
@REM echo Deal with BOOST location >> %TMPLOG%
@REM set BOOST_INCLUDEDIR=%TMPBOOST%\include\boost-1_53
@REM echo Added ENV BOOST_INCLUDEDIR=%BOOST_INCLUDEDIR% >> %TMPLOG%
@REM set BOOST_ROOT=%TMPBOOST%
@REM echo Added ENV BOOST_ROOT=%BOOST_ROOT% >> %TMPLOG%
@REM set BOOST_LIBRARYDIR=%TMPBOOST%\lib
@REM echo Added ENV BOOST_LIBRARYDIR=%BOOST_LIBRARYDIR% >> %TMPLOG%
@REM ##### if really need to see what FindBost.cmake is doing, add this
@REM set TMPOPTS=%TMPOPTS% -DBoost_DEBUG=1
@REM echo Added -DBoost_DEBUG=1 to add boost search output >> %TMPLOG%
@REM echo. >> %TMPLOG%

@echo Doing: 'cmake %TMPSRC% %TMPOPTS%'
@echo Doing: 'cmake %TMPSRC% %TMPOPTS%' >> %TMPLOG%
@cmake %TMPSRC% %TMPOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1
@REM echo.
@echo Check %TMPLOG% to ensure 3rdparty items found - *** Only Ctrl+C aborts ***
@REM echo.
@REM %DOPAUSE%

@echo Doing: 'cmake --build . --config debug'
@echo Doing: 'cmake --build . --config debug' >> %TMPLOG%
@cmake --build . --config debug >> %TMPLOG%
@if ERRORLEVEL 1 goto ERR2

@echo Doing: 'cmake --build . --config release'
@echo Doing: 'cmake --build . --config release' >> %TMPLOG%
@cmake --build . --config release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3
:DNREL

@echo.
@echo Appears a successful build
@echo.
@echo No Windows install at this time of this test application
@echo Perhaps with modification updexe.bat can be used to copy to a runtime location...
@echo.
@goto END
@echo Note install location %TMPINST%
@echo *** CONTINUE with install? *** Only Ctrl+C aborts ***
@echo.
@%DOPAUSE%

@echo Doing: 'cmake --build . --config debug --target INSTALL'
@echo Doing: 'cmake --build . --config debug --target INSTALL' >> %TMPLOG%
@cmake --build . --config debug --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4

@echo Doing: 'cmake --build . --config release --target INSTALL'
@echo Doing: 'cmake --build . --config release --target INSTALL' >> %TMPLOG%
@cmake --build . --config release --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5
@echo.
@REM to 'see' what was installed, add this
@fa4 " -- " %TMPLOG%
@echo.
@echo Done build and install of %TMPPRJ%...
@echo.
@goto END

:NOSRC
@echo Error: Can NOT locate SOURCE %TMPSRC%! *** FIX ME ***
@goto ISERR

:NOSG
@echo Error: Can NOT locate SG %TMPSG%! *** FIX ME ***
@goto ISERR

:NOCM
@echo Error: Can NOT locate FILE %TMPSRC%\CMakeLists.txt! *** FIX ME ***
@goto ISERR


:NOBAT
@echo Can NOT locate MSVC setup batch "%SET_BAT%"! *** FIX ME ***
@goto ISERR

:ERR0
@echo MSVC 10 setup error
@goto ISERR

:ERR1
@echo cmake config, generation error
@goto ISERR

:ERR2
@echo debug build error
@goto ISERR

:ERR3
@fa4 "mt.exe : general error c101008d:" %TMPLOG% >nul
@if ERRORLEVEL 1 goto ERR32
:ERR33
@echo release build error
@goto ISERR
:ERR32
@echo Stupid error... trying again...
@echo Doing: 'cmake --build . --config release'
@echo Doing: 'cmake --build . --config release' >> %TMPLOG%
@cmake --build . --config release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR33
@goto DNREL

:ERR4
@echo debug install error
@goto ISERR

:ERR5
@echo release install error
@goto ISERR

:NO3RD
@echo Error: Unable to locate %TMP3RD%! *** FIX ME ***
@echo Error: Unable to locate %TMP3RD%! *** FIX ME *** >> %TMPLOG%
@goto ISERR

:NOOSG
@echo Error: Unable to locate %TMPOSG%! *** FIX ME ***
@echo Error: Unable to locate %TMPOSG%! *** FIX ME *** >> %TMPLOG%
@goto ISERR

:NOBOOST
@echo Error: Unable to locate boost %TMPBOOST%! *** FIX ME ***
@echo Error: Unable to locate boost %TMPBOOST%! *** FIX ME *** >> %TMPLOG%
@goto ISERR

:ISERR
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
