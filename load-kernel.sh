#!/bin/sh
ROOT=`pwd`

image="$ROOT/romfs/boot/linux.bin"
#load_addr="0x60400000"

openocd -f "$ROOT/vendors/Elster/uwic/uwic-jtag.cfg" -c "load_sdram $image" || exit 1

echo == Image loaded ==