#!/bin/bash

ROOT=`pwd`

rm -rf /nfsroot/*

cp -r $ROOT/romfs/* /nfsroot/

find $ROOT/user/mtd-utils/ubi-utils -executable -type f -not -name '*.gdb' | xargs -I {} cp {} /nfsroot/bin

cp -r $ROOT/images /nfsroot/

mkdir -p /tmp/uwic-image
mount $ROOT/images/Image.ext2 /tmp/uwic-image
cp -r /tmp/uwic-image/dev /nfsroot/
umount /tmp/uwic-image


