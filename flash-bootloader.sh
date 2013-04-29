#!/bin/bash

ROOT=`pwd`
image="$ROOT/images/uboot.img"
rom_disk="$ROOT/images/romdisk.img"
openocd -f "$ROOT/vendors/Elster/uwic/uwic-jtag.cfg" -c "flash_mcu $image $rom_disk" || exit 1

echo "== Flashed =="
