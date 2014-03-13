#!/bin/sh

default_host="10.65.100.176"
read -p "Enter tftp host [$default_host]: " host
if [ "$host" == "" ]; then
	host=$default_host
fi

flash=/media/flash

mkdir -p $flash/boot
mkdir -p $flash/opt
mkdir -p $flash/etc/config
mkdir -p $flash/var/log
mkdir -p $flash/var/upgrade

echo "Downloading firmware from '$host'..."

echo "Loading /linux.bin ..."
tftp -g -l $flash/var/upgrade/kernel -r /linux.bin "$host"
sync

echo "Loading /initrd.bin ..."
tftp -g -l $flash/var/upgrade/initrd -r /initrd.bin "$host"
sync

mv $flash/var/upgrade/kernel   $flash/boot/linux.bin
mv $flash/var/upgrade/initrd   $flash/boot/initrd.bin
sync