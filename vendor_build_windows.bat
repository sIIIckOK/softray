@echo off

make -C .\vendor\raylib\src\

MOVE .\vendor\raylib\src\libraylib.a .\lib\windows

