:: enviroment config
call .\env_config.bat

:: build
cd %~dp0\..
mkdir build\release_mingw
cd build\release_mingw
cmake -G"MinGW Makefiles" ^
-DCMAKE_BUILD_TYPE=Release ^
%~dp0\..\code

:: make
mingw32-make

pause