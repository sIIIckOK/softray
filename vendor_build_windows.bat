<<<<<<< HEAD
mingw32-make -C ./vendor/raylib/src/

MOVE ".\vendor\raylib\src\libraylib.a" ".\lib\windows\libraylib.a"
=======
@echo off

SET CC=gcc
SET CFLAGS=-DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
SET LINK=-I.\external\glfw\include

PUSHD .\vendor\raylib\src\
%CC% %CFLAGS% -c *.c %LINK%
ar rcs libraylib.a *.o
POPD

MOVE .\vendor\raylib\src\libraylib.a .\lib\windows

>>>>>>> e4dc7a9a5c2b357b409134b6ba224315913d1c0d
