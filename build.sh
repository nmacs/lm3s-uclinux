#!/bin/bash


ROOT=`pwd`
TOOLCHAIN="$ROOT/../toolchain/uclinux/arm-2011.03"
TOOLCHAIN_BIN="$TOOLCHAIN/bin"
UBOOT_TOOLS_BIN="$ROOT/../u-boot-uwic/tools"
CC1_PATH="$TOOLCHAIN/libexec/gcc/arm-uclinuxeabi/4.5.2/"
export PATH=$TOOLCHAIN_BIN:$UBOOT_TOOLS_BIN:$CC1_PATH:$PATH

SYS_INSTALLER="sudo apt-get install"

rm -rf ./romfs

if [[ "$1" = "packages" ]]; then
	packages="git git-svn openocd libncurses5-dev xutils-dev libgmp-dev libmpfr-dev libmpc-dev lzop u-boot-tools mtd-utils putty genext2fs cmake libglib2.0-dev libtool autoconf fakeroot"
	$SYS_INSTALLER $packages
	exit 0
fi

if [[ "$1" = "repo" ]]; then
	make repo CROSS=arm-uclinuxeabi- CROSS_COMPILE=arm-uclinuxeabi-
	exit 0
fi

if [[ "$1" = "rebuild" ]]; then
  make clean CROSS=arm-uclinuxeabi- CROSS_COMPILE=arm-uclinuxeabi- || exit 1
fi

make TOOLCHAIN=$TOOLCHAIN CROSS_COMPILE=arm-uclinuxeabi- tools automake subdirs linux_image || exit 1
fakeroot -- make TOOLCHAIN=$TOOLCHAIN CROSS_COMPILE=arm-uclinuxeabi- romfs image || exit 1

echo == Compiled ==
