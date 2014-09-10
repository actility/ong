#!/bin/sh
#-----------------------------------------------------------------------------
#
#	mkshlib.sh
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
FILELIB=""
PROC=DoShLib

Usage()
{
	exec >&2

	echo "Usage: mkshlib.sh -p product -l lib"
	exit 1
}

Error()
{
	echo "mkshlib.sh: $1" >&2
}

ErrorExit()
{
	echo "mkshlib.sh: $1" >&2
	exit 1
}

DoShLib()
{
	echo "cp $FILELIB $ROOTACT/shlib"
	cp $PRODUCT/$FILELIB $ROOTACT/shlib
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
		-l)	shift
			if [ $# = 0 ]
			then
				Usage
			fi
			FILELIB=$1
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
if [ -z "$FILELIB" ]
then
	Error "library name not specified"
	Usage
fi

mkdir -p $ROOTACT/shlib > /dev/null 2>&1

cd $ROOTACT/$PRODUCT || ErrorExit "ROOTACT/$PRODUCT does not exist"

if [ ! -f "./Version" ]
then
	ErrorExit "./Version file not found"
fi
VERSION=`cat ./Version`
cd ..


$PROC


