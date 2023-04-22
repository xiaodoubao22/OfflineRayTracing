:: enviroment config
call .\env_config.bat

:: build
cd %~dp0\..
mkdir build\msvc_sln
cd build\msvc_sln
cmake -G"Visual Studio 17 2022"
%~dp0\..\code

pause
