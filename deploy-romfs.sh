#!/bin/bash

ROOT=`pwd`

deploy_list=( bin etc lib )
romfs="$ROOT/romfs"

for item in "${deploy_list[@]}"; do
  rm -rf /nfsroot/$item
  cp -r $romfs/$item /nfsroot/
done

cp -r $ROOT/romfs /nfsroot/

find $ROOT/user/mtd-utils/ubi-utils -executable -type f -not -name '*.gdb' | xargs -I {} cp {} /nfsroot/bin
