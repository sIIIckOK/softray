@echo off

SET CC=gcc
SET CFLAGS=-DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
SET LINK=-I.\external\glfw\include

PUSHD .\vendor\raylib\src\
%CC% %CFLAGS% -c *.c %LINK%
ar rcs libraylib.a *.o
POPD

MOVE .\vendor\raylib\src\libraylib.a .\lib\windows

