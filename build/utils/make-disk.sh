#!/bin/bash

DISK="./bin/loader/disk.hdd"

LOOP_DISK="`losetup -f`"
echo "using loop disk $LOOP_DISK"

if [ -f "$DISK" ]; then
    echo "disk exists, not remaking it."
	losetup -P $LOOP_DISK $DISK
else
	echo "making disk "
	dd if=/dev/zero of=$DISK bs=512 count=524288

	parted $DISK <<- EOF
	mktable gpt
	Yes
	mkpart 1 fat32 0 64
	I
	mkpart 2 ext2 64 200
	set 1 boot on
	quit
	EOF

	losetup -P $LOOP_DISK $DISK

	mkfs.ext2 $LOOP_DISK\p2
	mkfs.fat -F 32 $LOOP_DISK\p1
fi

P1_MNT_PATH="./bin/mnt_uefi"
mkdir -p $P1_MNT_PATH
mount $LOOP_DISK\p1 $P1_MNT_PATH

cp -r ./bin/loader/image/* $P1_MNT_PATH/

sync
umount $P1_MNT_PATH
losetup -d $LOOP_DISK
rm -rf $P1_MNT_PATH

