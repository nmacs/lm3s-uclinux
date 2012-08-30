#!/bin/bash

ROOT=`pwd`

deploy_list=( bin etc lib )
romfs="$ROOT/romfs"

rm -rf /nfsroot/*

for item in "${deploy_list[@]}"; do
  rm -rf /nfsroot/$item
  cp -r $romfs/$item /nfsroot/
done

cp -r $ROOT/romfs /nfsroot/

find $ROOT/user/mtd-utils/ubi-utils -executable -type f -not -name '*.gdb' | xargs -I {} cp {} /nfsroot/bin

cp -r $ROOT/images /nfsroot/

mkdir -p /tmp/uwic-image
mount $ROOT/images/Image.ext2 /tmp/uwic-image
cp -r /tmp/uwic-image/dev /nfsroot/
umount /tmp/uwic-image


