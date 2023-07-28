#!/bin/sh

DEPS="raylib"
CFLAGS="-Wextra -Wall -pedantic -std=c11 `pkg-config --cflags ${DEPS}`"
LIBS="`pkg-config --libs ${DEPS}` -lm"

clang $CFLAGS -o scaling scaling.c $LIBS
clang $CFLAGS -o adding adding.c $LIBS
