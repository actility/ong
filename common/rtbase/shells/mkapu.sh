#!/bin/sh
#-----------------------------------------------------------------------------
#
#	mkapu.sh
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
PROC=DoApu

Usage()
{
	exec >&2

	echo "Usage: mkapu.sh -p product"
	exit 1
}

Error()
{
	echo "mkapu.sh: $1" >&2
}

ErrorExit()
{
	echo "mkapu.sh: $1" >&2
	exit 1
}

DoApu()
{
echo	$SYSTEM $ROOTACT
echo	$PRODUCT $VERSION

$ROOTACT/rtbase/shells/mkdep.sh -p $PRODUCT

#echo	"$ROOTACT/apu-tools/apu-maker.sh -l $SYSTEM -s"
$ROOTACT/apu-tools/apu-maker.sh -l $SYSTEM -s
#apu-maker.sh -l $SYSTEM -s
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
if [ ! -f "$ROOTACT/rtbase/base/TARGET" ]
then
	ErrorExit "ROOTACT:${ROOTACT} not targeted"
fi
source $ROOTACT/rtbase/base/TARGET


if [ ! -f "$ROOTACT/apu-tools/apu-maker.sh" ]
then
	ErrorExit "apu tools not installed"
fi

cd $ROOTACT/$PRODUCT || ErrorExit "ROOTACT/$PRODUCT does not exist"

mkdir -p $ROOTACT/apu-repository > /dev/null 2>&1

if [ ! -f "./Version" ]
then
	ErrorExit "./Version file not found"
fi
VERSION=`cat ./Version`


$PROC

exit 0


