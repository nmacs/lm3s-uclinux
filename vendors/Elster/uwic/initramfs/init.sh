#!/bin/sh

PATH="/bin"

mount -t proc proc /proc

read -t 5 -p "Enter 'up' to upgrade firmware or 'nfs' for nfs boot: " cmd
if [ "$cmd" == "nfs" ]; then
	echo "Mounting nfs as root..."
	mount -t nfs -o port=2049,nolock,proto=tcp,vers=2 10.65.100.176:/nfsroot /newroot
else
	if [ "$cmd" == "up" ]; then
		mount -t sysfs sys /sys
		echo "Mounting nfs..."
		mount -t nfs -o port=2049,nolock,proto=tcp,vers=2 10.65.100.176:/nfsroot /mnt
		echo "Flashing..."
		/mnt/bin/start-stop-daemon -x /mnt/bin/watchdogd -p /tmp/watchdogd.pid -m -b -S -- -f
		/mnt/bin/ubiupdatevol /dev/ubi0_0 /mnt/images/firmware.ubifs
		/mnt/bin/start-stop-daemon -p /tmp/watchdogd.pid -m -b -K
		umount /mnt
		umount /sys
		echo "Done"
	fi
	echo "Mounting ramfs as root..."
	mount -t ramfs ramfs /newroot

	mkdir /newroot/opt
	mkdir /newroot/media
	mkdir /newroot/media/flash
	mkdir /newroot/tmp
	mkdir /newroot/proc
	mkdir /newroot/sys
	mkdir /newroot/var
	mkdir /newroot/var/run

	ln -s /opt/dev /newroot/dev
	ln -s /opt/bin /newroot/bin
	ln -s /opt/etc /newroot/etc
	ln -s /opt/usr /newroot/usr
	ln -s /media/flash/var/log /newroot/var/log
	ln -s /opt/www /newroot/www
	
	echo "Mounting ubifs to /media/flash..."
	mount -t ubifs /dev/ubi0_0 /newroot/media/flash
	
	echo "Mounting distribution..."
	mount -t cramfs -o loop /newroot/media/flash/opt/distribution.cramfs /newroot/opt
fi

echo 3 > /proc/sys/vm/drop_caches

umount /proc

echo "Switching to the new root..."
exec switch_root /newroot /bin/init