#!/bin/bash

package=$1
repo=$2
codename=$3
arch=$4
saveto=$5

rm -rf /tmp/opkg
mkdir -p /tmp/opkg/usr/lib/opkg

cat > /tmp/opkg/opkg.conf << EOF
dist $codename $repo
dest root /
EOF

cd /tmp/opkg
opkg-cl -f /tmp/opkg/opkg.conf --add-arch $arch:1 -o /tmp/opkg update
opkg-cl -f /tmp/opkg/opkg.conf --add-arch $arch:1 -o /tmp/opkg download $package

mv $package*.deb $saveto
rm -rf /tmp/opkg