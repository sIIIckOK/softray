set -xe

mkdir -p ./bin/

gcc ./platform/plat_raylib.c -o ./bin/raylib_plat -I./include/ -L./lib/ -lraylib -lm

