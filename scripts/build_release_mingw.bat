:: enviroment config
call .\env_config.bat

:: build
cd %~dp0\..
mkdir build\release_mingw
cd build\release_mingw
cmake -G"MinGW Makefiles" ^
-DCMAKE_BUILD_TYPE=Release ^
-DQt5_PREFIX_PATH=%qt5_prefix_path% ^
%~dp0\..\code

:: make
mingw32-make

:: copy libs
call %qt_mingw_bin_path%/qtenv2.bat
cd /d %script_dir%/../build/release_mingw
windeployqt PathTracing.exe

pause