#!/bin/sh

set -xe

DEPS="raylib"
CFLAGS="-Wextra -Wall -pedantic -std=c11 `pkg-config --cflags ${DEPS}`"
LIBS="`pkg-config --libs ${DEPS}` -lm"

mkdir -p demos

clang -DGEOLIB_ADDING $CFLAGS -o demos/adding main.c $LIBS
clang -DGEOLIB_SUBTRACTING $CFLAGS -o demos/subtracting main.c $LIBS
clang -DGEOLIB_ROTATING $CFLAGS -o demos/rotating main.c $LIBS
clang -DGEOLIB_SCALING $CFLAGS -o demos/scaling main.c $LIBS
