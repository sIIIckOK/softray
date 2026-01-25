set -xe

mkdir -p ./bin/

gcc ./platform/plat_raylib.c -o ./bin/plat_raylib_linux -I./include/ -L./lib/linux/ -lraylib -lm

