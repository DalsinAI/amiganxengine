#!/bin/sh
# Lab tools for the lab runs (README, "Observed"); not part of the port.
# Cradle's stove_build target "tests/lab" (STOVE= and OUT= set).
#   JoyPort   both game ports as lowlevel.library reads them, every change
#   HoldKeys  holds keys down for a time, or in turn (Cradle's input tool
#             only taps)
#   Chop      cuts a file (a screenshot) into pieces small enough to fetch
#   Grab      the front screen, or a part of it, as a PNG file (libpng and
#             zlib: the codec inventory staged in the stove)
# JoyPort, HoldKeys and Chop as DalsinAI/amigakobodeluxe's tests/lab has
# them (JoyPort and HoldKeys from amigapowermanga).
# MIT, Copyright (c) 2026 AmiCradle Team.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
if [ -z "${STOVE:-}" ]; then
    STOVE="$HOME/AmiCradle/stoves/os32-gcc16"
    [ -d "$STOVE" ] || STOVE="$HOME/AmigaChrome/stoves/os32-gcc16"
fi
OUT=${OUT:-"$HERE/build-amiga"}
CC="$STOVE/prefix/bin/m68k-amigaos-gcc"
mkdir -p "$OUT"
"$CC" --version | head -n 1
for t in JoyPort:joyport HoldKeys:holdkeys Chop:chop; do
    "$CC" -noixemul -m68040 -O2 -Wall -fno-delete-null-pointer-checks \
        -ffile-prefix-map="$HERE"=. -o "$OUT/${t%%:*}" "$HERE/${t#*:}.c"
done
DEPS=$(python3 "$HERE/../../scripts/port_dependencies.py" verify --stove "$STOVE" --require zlib png)
"$CC" -noixemul -m68040 -O2 -Wall -fno-delete-null-pointer-checks \
    -ffile-prefix-map="$HERE"=. -I"$DEPS/include" -o "$OUT/Grab" "$HERE/grab.c" \
    -L"$DEPS/lib" -lpng -lz -lm
sha256sum "$OUT/JoyPort" "$OUT/HoldKeys" "$OUT/Chop" "$OUT/Grab"
