#!/bin/bash

root=`pwd`
toolset="$root/../../../toolchain/uclinux/arm-2011.03/bin"
#export PATH="$toolset:$PATH"

CROSSDEV="$toolset/arm-uclinuxeabi"

export CC="$CROSSDEV-gcc"
export CXX="$CROSSDEV-g++"
export CPP="$CROSSDEV-gcc -E"
export LD="$CROSSDEV-ld"
export AR="$CROSSDEV-ar"
export NM="$CROSSDEV-nm"
export OBJCOPY="$CROSSDEV-objcopy"
export OBJDUMP="$CROSSDEV-objdump"

export CFLAGS="-mtune=cortex-m3 -march=armv7-m -mthumb"
export LDFLAGS="-mtune=cortex-m3 -march=armv7-m -mthumb"

make clean
./configure --host=arm-uclinux --disable-curl --disable-gpg --prefix="$root/output"
make
make install
