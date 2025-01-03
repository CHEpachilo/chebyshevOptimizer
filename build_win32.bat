@REM Build for Visual Studio compiler. Run your copy of vcvars64.bat or vcvarsall.bat to setup command-line compiler.
@set OUT_DIR=Debug
@set OUT_EXE=chebOptimizer
@set INCLUDES=/I libs\imgui /I libs\imgui\backends /I libs\imgui\examples\libs\glfw\include
@set SOURCES=main.cpp libs\imgui\backends\imgui_impl_glfw.cpp libs\imgui\backends\imgui_impl_opengl3.cpp libs\imgui\imgui*.cpp
@set LIBS=/LIBPATH:libs\imgui\examples\libs\glfw\lib-vc2010-64 glfw3.lib opengl32.lib gdi32.lib shell32.lib
@ECHO off
mkdir %OUT_DIR% >NUL 2>&1
cl /std:c++latest /nologo /Zi /MD /utf-8 %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS% /SUBSYSTEM:WINDOWS
