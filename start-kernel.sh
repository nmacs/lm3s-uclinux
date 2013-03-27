#!/bin/sh
ROOT=`pwd`

tty=$1

echo == Starting kernel... ==

./reset-mcu.sh
echo -n 1 > "$tty"
sleep 1
echo "run nfsboot" > "$tty"

echo == Started ==
