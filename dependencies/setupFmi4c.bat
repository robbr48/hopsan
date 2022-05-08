@ECHO OFF

REM Bat script for building fmi4c dependency automatically

setlocal
set basedir=%~dp0
set name=fmi4c
set codedir=%basedir%\%name%-code
set builddir=%basedir%\%name%-build
set installdir=%basedir%\%name%

REM Setup PATH
call setHopsanBuildPaths.bat

REM build
if exist %builddir% (
  echo Removing existing build directory %builddir%
  rmdir /S /Q %builddir%
)
mkdir %builddir%
cd %builddir%
cmake -Wno-dev -G %HOPSAN_BUILD_CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%installdir% -DBUILD_SHARED=OFF -DUSE_INCLUDED_ZLIB=ON %codedir%
cmake --build . --config Release --parallel 8
cmake --build . --config Release --target install

cd %basedir%
echo.
echo setupFmi4c.bat done!"
if "%HOPSAN_BUILD_SCRIPT_NOPAUSE%" == "" (
  pause
)
endlocal