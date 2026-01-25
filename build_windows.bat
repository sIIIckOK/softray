@echo off

if not exist "bin" mkdir "bin"

gcc ./platform/plat_raylib.c -I./include/ -L./lib/windows/ -o ./bin/plat_raylib_windows -lraylib -lm -lgdi32 -lwinmm -lopengl32



