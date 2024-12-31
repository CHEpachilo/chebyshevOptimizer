@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
@set OUT_DIR=Debug
@set OUT_EXE=benchmark
@set INCLUDES=
@set SOURCES=main.cpp
@set LIBS=/LIBPATH:libs gdi32.lib shell32.lib
mkdir %OUT_DIR%
cl /std:c++latest /nologo /Zi /MD /utf-8 %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/
