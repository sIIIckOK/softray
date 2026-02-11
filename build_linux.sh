set -xe

mkdir -p ./bin/

gcc -fsanitize=address -o3 ./platform/plat_raylib.c -o ./bin/plat_raylib_linux -I./include/ -L./lib/linux/ -lraylib -lm

