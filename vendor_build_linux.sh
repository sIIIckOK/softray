#!/usr/bin/bash

set -xe

make -C ./vendor/raylib/src/

mv ./vendor/raylib/src/libraylib.a ./lib/linux/libraylib.a

