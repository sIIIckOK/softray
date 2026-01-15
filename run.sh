set -xe

mkdir -p ./bin/

gcc ./platform/plat_raylib.c -o ./bin/plat_raylib -I./include/ -L./lib/ -lraylib -lm

