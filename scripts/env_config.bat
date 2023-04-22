:: 当前脚本路径
set script_dir=%~dp0

:: # Qt5前缀路径
set qt5_prefix_path=C:/Qt/Qt5.14.2/5.14.2/mingw73_64/lib/cmake
@REM set qt5_prefix_path=C:\Qt\Qt5.14.2\5.14.2\msvc2017_64\lib\cmake

:: qt自带的mingw的相关工具路径（不是MinGW路径）
set qt_mingw_bin_path=C:/Qt/Qt5.14.2/5.14.2/mingw73_64/bin

:: MinGW路径
set mingw_path=C:/Qt/Qt5.14.2/Tools/mingw730_64/bin

:: 设置环境变量
set PATH=%mingw_path%;%PATH%