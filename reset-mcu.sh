#!/bin/sh
ROOT=`pwd`

image="$ROOT/romfs/boot/linux.bin"

tools/openocd -f "$ROOT/vendors/Elster/uwic/uwic-jtag.cfg" -c "reset_mcu" || exit 1

echo == MCU Reset OK ==
