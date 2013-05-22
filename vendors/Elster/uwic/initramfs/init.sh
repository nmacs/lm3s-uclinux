#!/bin/sh

PATH="/bin"

mount -t proc proc /proc
mount -t cramfs /dev/mtdblock0 /romdisk

read -t 5 -p "Upgrade firmware [y/N]: " answer

if [ "$answer" == "y" ]; then
	mount -t sysfs sys /sys
	start-stop-daemon -x watchdogd -p /tmp/watchdogd.pid -m -b -S -- -f
	ifconfig eth0 10.65.100.205 netmask 255.255.255.0

	default_url="http://10.65.100.176/firmware.ubifs"
	read -p "Enter firmware URL [$default_url]: " url
	if [ "$url" == "" ]; then
		url=$default_url
	fi

	echo "Downloading firmware from '$url'..."
	wget -O /tmp/firmware -T 10 $url
	
	echo "Flashing..."
	ubiupdatevol /dev/ubi0_0 /tmp/firmware

	rm /tmp/firmware
	umount /sys
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
ln -s /media/flash/var /newroot/var
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

if [ -f /tmp/watchdogd.pid ]; then
	start-stop-daemon -p /tmp/watchdogd.pid -m -b -K
fi

echo "Switching to the new root..."
exec switch_root /newroot /bin/init