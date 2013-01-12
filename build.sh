#!/bin/bash


ROOT=`pwd`
TOOLCHAIN="$ROOT/../toolchain/uclinux/arm-2011.03"
TOOLCHAIN_BIN="$TOOLCHAIN/bin"
UBOOT_TOOLS_BIN="$ROOT/../u-boot-uwic/tools"
CC1_PATH="$TOOLCHAIN/libexec/gcc/arm-uclinuxeabi/4.5.2/"
export PATH=$TOOLCHAIN_BIN:$UBOOT_TOOLS_BIN:$CC1_PATH:$PATH

rm -rf ./romfs

if [[ "$1" = "rebuild" ]]; then
  make clean CROSS_COMPILE=arm-uclinuxeabi-
fi

make TOOLCHAIN=$TOOLCHAIN CROSS_COMPILE=arm-uclinuxeabi- || exit 1

echo == Compiled ==
