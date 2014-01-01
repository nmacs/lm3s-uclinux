#!/bin/bash

ROOT=`pwd`

rm -rf /nfsroot/*

cp -r $ROOT/romfs/* /nfsroot/

cp -r $ROOT/images /nfsroot/
cp $ROOT/romfs/boot/linux.bin /tftpboot/
cp $ROOT/images/distribution.cramfs /tftpboot/
cp $ROOT/images/distribution.cramfs.sig /tftpboot/

mkdir -p /tmp/uwic-image
mount $ROOT/images/Image.ext2 /tmp/uwic-image
cp -r /tmp/uwic-image/dev /nfsroot/
umount /tmp/uwic-image


