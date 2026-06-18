<<<<<<< HEAD
if not exist ".\bin" (
    mkdir ".\bin"
)

gcc -o3 ./platform/plat_raylib.c -o ./bin/plat_raylib_windows -I./include/ -L./lib/windows/ -lraylib -lm -lgdi32 -lwinmm -lopengl32 -limm32 -lole32 -luuid -lcomdlg32 -lshell32
=======
@echo off

if not exist "bin" mkdir "bin"

gcc ./platform/plat_raylib.c -I./include/ -L./lib/windows/ -o ./bin/plat_raylib_windows -lraylib -lm -lgdi32 -lwinmm -lopengl32



>>>>>>> e4dc7a9a5c2b357b409134b6ba224315913d1c0d
