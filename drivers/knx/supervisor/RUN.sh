#!/bin/sh -x

export LD_LIBRARY_PATH=../../shlib:../contrib/root/lib
killall knx.x

if [ -f main.c ]
then
	./MAKE || exit 1
fi

mv stdouterr.log stdouterr.old > /dev/null 2>&1
rm TRACE*.log core core.* > /dev/null 2>&1
./knx.x $* > stdouterr.log 2>&1 &
sleep 2 
tail -n 1000 -f ./TRACE.log
