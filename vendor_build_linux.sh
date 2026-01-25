#!/usr/bin/bash

set -xe

make -C ./vendor/raylib/src/
rm ./vendor/raylib/src/*.o

mv ./vendor/raylib/src/libraylib.a ./lib/linux/libraylib.a

