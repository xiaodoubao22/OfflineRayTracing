:: enviroment config
call ./env_config.bat

:: build
cd %~dp0/..
mkdir -p build\debug_mingw
cd build/debug_mingw
cmake -G"MinGW Makefiles" ^
-DCMAKE_BUILD_TYPE=Debug ^
%~dp0/../code

:: make
mingw32-make

pause

