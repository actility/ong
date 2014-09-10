#!/bin/sh
#-----------------------------------------------------------------------------
#
#	mktar.sh
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
PROC=DoTar

Usage()
{
	exec >&2

	echo "Usage: mkruntime.sh -p product"
	exit 1
}

Error()
{
	echo "mkruntime.sh: $1" >&2
}

ErrorExit()
{
	echo "mkruntime.sh: $1" >&2
	exit 1
}

DoTar()
{
echo "	tar cf $ROOTACT/deliveries/${PRODUCT}-${VERSION}-${SYSTEM}.tar -T ${LISTFILES}"
	tar cf $ROOTACT/deliveries/${PRODUCT}-${VERSION}-${SYSTEM}.tar -T ${LISTFILES}
}

while [ $# -gt 0 ]
do
	case $1 in
		-p)	shift
			if [ $# = 0 ]
			then
				Usage
			fi
			PRODUCT=$1
			shift
		;;
	esac
done

if [ -z "$ROOTACT" ]
then
	Error "ROOTACT not specified"
	Usage
fi
if [ -z "$PRODUCT" ]
then
	Error "product name not specified"
	Usage
fi

. $ROOTACT/rtbase/base/system

mkdir -p $ROOTACT/deliveries > /dev/null 2>&1



cd $ROOTACT/$PRODUCT || ErrorExit "ROOTACT/$PRODUCT does not exist"

if [ ! -f "./Version" ]
then
	ErrorExit "./Version file not found"
fi
VERSION=`cat ./Version`
cd ..
#echo "rtbase/MAKE" > /tmp/XX.lst
#LISTFILES=/tmp/XX.lst
LISTFILES=$ROOTACT/$PRODUCT/package/runtime.lst
if [ ! -f $LISTFILES ]
then
	if [ -f $ROOTACT/rtbase/package/$PRODUCT-runtime.lst ]
	then
	LISTFILES=$ROOTACT/rtbase/package/$PRODUCT-runtime.lst
	Error "use special runtime list from rtbase/package for $PRODUCT"
	else
	LISTFILES=/tmp/$PRODUCT-${VERSION}-${SYSTEM}.lst
	Error "./package/runtime.lst file not found (default list created !!!)"
	find $PRODUCT -type f ! -name "*.[clyo]" -a ! -name "[Mm]akefile" | grep -v '.git' > $LISTFILES
	fi
fi


$PROC


