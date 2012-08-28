#!/bin/bash


ROOT=`pwd`
TOOLCHAIN_BIN="$ROOT/../toolchain/uclinux/arm-2011.03/bin"
UBOOT_TOOLS_BIN="$ROOT/../u-boot-uwic/tools"
CC1_PATH="$ROOT/../toolchain/uclinux/arm-2011.03/libexec/gcc/arm-uclinuxeabi/4.5.2/"
export PATH=$TOOLCHAIN_BIN:$UBOOT_TOOLS_BIN:$CC1_PATH:$PATH

if [[ "$1" = "rebuild" ]]; then
  make clean CROSS_COMPILE=arm-uclinuxeabi-
fi

make CROSS_COMPILE=arm-uclinuxeabi- || exit 1

echo == Compiled ==
