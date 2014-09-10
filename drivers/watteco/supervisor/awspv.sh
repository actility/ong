#!/bin/sh
LD_LIBRARY_PATH=$ROOTACT/lib:$ROOTACT/shlib
export LD_LIBRARY_PATH

#for devel installation
#options="--dev"

killall awspv.x > /dev/null 2>&1
cd $ROOTACT/watteco/supervisor
mv stdouterr.log stdouterr.old.log > /dev/null 2>&1
exec ./awspv.x $options > stdouterr.log 2>&1 &
