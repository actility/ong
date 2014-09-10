#!/bin/sh
#-----------------------------------------------------------------------------
#
#	mkwhat.sh
#
#	Construction d'une what-string ACT
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
PROC=WhatC

Usage()
{
	exec >&2

	echo "Usage: mkwhat.sh [-sh] -p product -v VERSION"
	echo "       mkwhat.sh [-sh] -p product -vf version_file"
	exit 1
}

Error()
{
	echo "mkwhat.sh: $1" >&2
}

WhatC()
{
	echo "static char *${PRODUCT}_whatStr=\"@(#) Actility ${PRODUCT} ${VERSION} ${DATE} target=${SYSTEM} host=${MACHINE}\";"
}

WhatSh()
{
	echo "@(#) Actility ${PRODUCT} ${VERSION} ${DATE} ${SYSTEM} ${MACHINE}"
}

WhatRc()
{
	echo "#define __Version ${VERSION}"
	echo "#define __VersStr \"${VERSION}\\\0\""
	echo "#define __Product \"${PRODUCT}\\\0\""
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
		-vf)	shift
			[ $# = 0 ] && Usage
			if [ ! -f $1 ]
			then
				Error "file 'Version' not found"
				exit 1
			fi
			read VERSION eol < $1
			shift
			;;
		-sh)	PROC=WhatSh
			shift
			;;
		-rc)	PROC=WhatRc
			shift
			;;
		-v)	shift
			if [ $# = 0 ]
			then
				Usage
			fi
			VERSION=$1
			shift
			;;
	esac
done

if [ ! -f "$ROOTACT/rtbase/base/TARGET" ]
then
	ErrorExit "ROOTACT:${ROOTACT} not targeted"
fi
source $ROOTACT/rtbase/base/TARGET

if [ -z "$PRODUCT" ]
then
	Error "product name not specified"
	Usage
fi

if [ -z "$VERSION" ]
then
	Error "version not specified" >&2
	Usage
fi

#ARCH=`uname -m`
#OS=`uname -o`
#SYSTEM=${OS}_${ARCH}

DATE=`date "+%d/%m/%y"`
MACHINE=`uname -n`

$PROC


