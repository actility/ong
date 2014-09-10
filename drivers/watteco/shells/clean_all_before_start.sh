#!/bin/sh
####!/bin/sh -x

if [ -z "$ROOTACT" ]
then
	echo "ROOTACT not set"
	exit 1
fi

if [ -z "$AW_DATA_DIR" ]
then
	AW_DATA_DIR=$ROOTACT/usr/data/watteco
	export AW_DATA_DIR
fi

if [ -f $AW_DATA_DIR/shells/clean_all_before_start.sh ]
then
	exec $AW_DATA_DIR/shells/clean_all_before_start.sh
fi

if [ -d $AW_DATA_DIR/knownsensors.previous ]
then
	rm -fr $AW_DATA_DIR/knownsensors.bkp
fi

mv $AW_DATA_DIR/knownsensors $AW_DATA_DIR/knownsensors.bkp
mv $AW_DATA_DIR/discoverpan.txt $AW_DATA_DIR/discoverpan.txt.bkp
mv $AW_DATA_DIR/discoversoft.txt $AW_DATA_DIR/discoversoft.txt.bkp

exit 0
