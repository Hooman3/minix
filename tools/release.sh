#!/bin/sh

COPYITEMS="usr/src usr/bin bin usr/lib"
RELEASEDIR=/usr/r/release
IMAGE=cdfdimage
ISO=minix.iso
RAM=/dev/ram
if [ `wc -c $RAM | awk '{ print $1 }'` -ne 1474560 ]
then	echo "$RAM should be exactly 1440k."
	exit 1
fi
echo "Warning: I'm going to mkfs $RAM!"
echo "Temporary (sub)partition to use to make the /usr FS image? "
echo "It has to be at least 40MB, not much more because it's going to be "
echo "appended to the .iso. It will be mkfsed!"
echo -n "Device: /dev/"
read dev || exit 1
TMPDISK=/dev/$dev

if [ -b $TMPDISK ]
then :
else	echo "$TMPDISK is not a block device.."
	exit 1
fi

umount $TMPDISK
umount $RAM

( cd .. && make clean )
rm -rf $RELEASEDIR $ISO $IMAGE 
mkdir -p $RELEASEDIR
mkfs -b 1440 -B 1024 $RAM || exit
echo " * mounting $RAM as $RELEASEDIR"
mount $RAM $RELEASEDIR || exit
mkdir $RELEASEDIR/usr

mkfs -B 1024 $TMPDISK
echo " * mounting $TMPDISK as $RELEASEDIR/usr"
mount $TMPDISK $RELEASEDIR/usr || exit
mkdir -p $RELEASEDIR/tmp
mkdir -p $RELEASEDIR/usr/tmp
echo " * Transfering $COPYITEMS to $RELEASEDIR"
( cd / && tar cf - $COPYITEMS ) | ( cd $RELEASEDIR && tar xf - ) || exit 1
echo " * Chroot build"
chroot $RELEASEDIR '/bin/sh -x /usr/src/tools/chrootmake.sh' || exit 1
echo " * Chroot build done"
umount $TMPDISK || exit
umount $RAM || exit
cp $RAM rootimage
make programs image
(cd ../boot && make)
make image || exit 1
./mkboot cdfdboot
writeisofs -l MINIX -b $IMAGE /tmp $ISO || exit 1
echo "Appending Minix root filesystem"
cat >>$ISO rootimage || exit 1
echo "Appending Minix usr filesystem"
cat >>$ISO $TMPDISK || exit 1
ls -al $ISO
