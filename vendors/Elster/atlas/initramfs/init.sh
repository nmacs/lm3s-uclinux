#!/bin/sh

PATH="/bin"

mount -t proc proc /proc
mount -t cramfs /dev/mtdblock0 /romdisk

read -t 5 -p "Upgrade firmware [y/N]: " answer

if [ "$answer" == "y" ]; then
	ifconfig eth0 10.65.100.205 netmask 255.255.255.0

	default_host="10.65.100.176"
	read -p "Enter tftp host [$default_host]: " host
	if [ "$host" == "" ]; then
		host=$default_host
	fi

	echo "Mounting ubifs to /mnt..."
	mount -t ubifs /dev/ubi0_0 /mnt

	mkdir -p /mnt/boot
	mkdir -p /mnt/opt
	mkdir -p /mnt/opt/apps
	mkdir -p /mnt/etc/config
	mkdir -p /mnt/var/log
	mkdir -p /mnt/var/upgrade

	echo "Downloading firmware from '$host'..."
	echo 1 > /dev/watchdog
	echo "Loading /linux.bin ..."
	tftp -g -l /mnt/var/upgrade/kernel -r /linux.bin "$host"
	sync
	echo 1 > /dev/watchdog
	echo "Loading /distribution.cramfs ..."
	tftp -g -l /mnt/var/upgrade/distr -r /distribution.cramfs "$host"
	sync
	echo 1 > /dev/watchdog
	echo "Loading /distribution.cramfs.sig ..."
	tftp -g -l /mnt/var/upgrade/distrsig -r /distribution.cramfs.sig "$host"
	sync
	sync
	echo 1 > /dev/watchdog

	echo "Moving files..."
	mv /mnt/var/upgrade/kernel   /mnt/boot/linux.bin
	mv /mnt/var/upgrade/distr    /mnt/opt/distribution.cramfs
	mv /mnt/var/upgrade/distrsig /mnt/opt/distribution.cramfs.sig
	sync
	sync
	echo 1 > /dev/watchdog

	umount /mnt
	echo "Done"
fi

echo "Mounting ramfs as root..."
mount -t ramfs ramfs /newroot

mkdir /newroot/opt
mkdir /newroot/media
mkdir /newroot/media/flash
mkdir /newroot/tmp
mkdir /newroot/tmp/run
mkdir /newroot/proc
mkdir /newroot/sys

ln -s /opt/dev /newroot/dev
ln -s /opt/bin /newroot/bin
ln -s /opt/etc /newroot/etc
ln -s /opt/usr /newroot/usr
ln -s /opt/var /newroot/var
ln -s /opt/www /newroot/www

echo "Mounting ubifs to /media/flash..."
mount -t ubifs /dev/ubi0_0 /newroot/media/flash

echo "Checking distribution signature..."
rsacheck /newroot/media/flash/opt/distribution.cramfs /newroot/media/flash/opt/distribution.cramfs.sig /romdisk/pub_key
check_result=$?
if [ "$check_result" != 0 ]; then
	echo "Tampering possible! Halt the system."
	exit 1
fi

echo "Mounting distribution..."
mount -t cramfs -o loop /newroot/media/flash/opt/distribution.cramfs /newroot/opt

echo 3 > /proc/sys/vm/drop_caches

umount /proc
umount /romdisk

echo "Switching to the new root..."
exec switch_root /newroot /bin/init