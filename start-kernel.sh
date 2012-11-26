#!/bin/sh
ROOT=`pwd`

image="$ROOT/romfs/boot/linux.bin"
#load_addr="0x60400000"

openocd -f "$ROOT/vendors/Elster/uwic/uwic-jtag.cfg" -c "start_kernel $image" || exit 1

echo == Image loaded ==

tty=$1

echo == Starting kernel... ==
echo -n 1 > "$tty"
sleep 1
echo setenv bootargs > "$tty"
echo bootm 0x60400000 > "$tty"

echo == Started ==
