#!/bin/sh

CC=clang

DEPS="raylib"
CFLAGS="-Wextra -Wall -pedantic -std=c11 `pkg-config --cflags ${DEPS}`"
LIBS="`pkg-config --libs ${DEPS}` -lm"

$CC $CFLAGS -o geolib main.c $LIBS
