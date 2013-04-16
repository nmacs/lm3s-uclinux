#!/bin/bash

package=$1
repo=$2
codename=$3
arch=$4
saveto=$5

rm -rf /tmp/opkg
mkdir -p /tmp/opkg/tmp

cat > /tmp/opkg/opkg.conf << EOF
dist $codename $repo
dest root /
EOF

cd /tmp/opkg
opkg-cl -f /tmp/opkg/opkg.conf --add-arch $arch:1 -o /tmp/opkg update || exit 1
opkg-cl -f /tmp/opkg/opkg.conf --add-arch $arch:1 -o /tmp/opkg download $package || exit 1

mv $package*.deb $saveto || exit 1
rm -rf /tmp/opkg