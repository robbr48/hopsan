@ECHO OFF

REM Bat script for building ZeroMQ dependency automatically

setlocal
set basedir=%~dp0
set name=zeromq
set codedir=%basedir%\%name%-code
set builddir=%basedir%\%name%-build
set installdir=%basedir%\%name%

REM Setup PATH
call setHopsanBuildPaths.bat

REM Apply build patch
set PATH=%PATH_WITH_MSYS%
cd %codedir%
patch.exe --forward -p1 < ..\zeromq-Prevent-conflicting-definition-of-close-when-compili.patch
set PATH=%PATH_WITHOUT_MSYS%

REM build
if exist %builddir% (
  echo Removing existing build directory %builddir%
  rmdir /S /Q %builddir%
)
mkdir %builddir%
cd %builddir%

if "%HOPSAN_BUILD_COMPILER%" == "msvc" (
  cmake -Wno-dev -DWITH_LIBSODIUM=OFF -G %HOPSAN_BUILD_CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=%installdir% %codedir%
  cmake --build . --config Debug --parallel 8
  cmake --build . --config Debug --target install
  cmake -Wno-dev -DWITH_LIBSODIUM=OFF -G %HOPSAN_BUILD_CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%installdir% %codedir%
  cmake --build . --config Release --parallel 8
  cmake --build . --config Release --target install
  if not "%HOPSAN_BUILD_DEPENDENCIES_TEST%" == "false" (
    ctest -C Release --parallel 8
  )
) else (
  cmake -Wno-dev -DWITH_LIBSODIUM=OFF -DZMQ_HAVE_IPC=0 -G %HOPSAN_BUILD_CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%installdir% %codedir%
  cmake --build . --parallel 8
  cmake --build . --target install
  if not "%HOPSAN_BUILD_DEPENDENCIES_TEST%" == "false" (
    ctest --parallel 8
  )
)

REM Now "install" cppzmq (header only), to the zmq install dir
REM TODO in the future use cmake to install cppzmq, but that does not work right now since no "zeromq-config.cmake" file seem to be created.
REM For now lets just copy the file
set codedir=%basedir%\cppzmq-code
set installdir=%installdir%\include\

xcopy /Y %codedir%\*.hpp %installdir%

cd %basedir%
echo.
echo setupZeroMQ.bat done!"
if "%HOPSAN_BUILD_SCRIPT_NOPAUSE%" == "" (
  pause
)
endlocal
