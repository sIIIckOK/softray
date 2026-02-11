if not exist ".\lib\windows" (
    mkdir ".\lib\windows"
)

gcc -o3 ./platform/plat_raylib.c -o ./bin/plat_raylib_windows -I./include/ -L./lib/windows/ -lraylib -lm -lgdi32 -lwinmm -lopengl32 -limm32 -lole32 -luuid -lcomdlg32 -lshell32
