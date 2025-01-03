@REM Build for Visual Studio compiler. Run your copy of vcvars64.bat or vcvarsall.bat to setup command-line compiler.
@set OUT_DIR=Debug
@set OUT_EXE=benchmark
@set INCLUDES=
@set SOURCES=main.cpp
@set LIBS=/LIBPATH:libs gdi32.lib shell32.lib
@ECHO off
mkdir %OUT_DIR% >NUL 2>&1
cl /std:c++latest /nologo /Zi /MD /utf-8 %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/
@ECHO off
xcopy %OUT_DIR%\%OUT_EXE%.exe %OUT_DIR%\..\ /y/q >NUL 2>&1
