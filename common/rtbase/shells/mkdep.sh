#!/bin/sh
#-----------------------------------------------------------------------------
#
#	mkdep.sh
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
PROC=DoDep

Usage()
{
	exec >&2

	echo "Usage: mkdep.sh -p product"
	exit 1
}

Error()
{
	echo "mkdep.sh: $1" >&2
}

ErrorExit()
{
	echo "mkdep.sh: $1" >&2
	exit 1
}

verlte()
{
	[  "$1" = "`echo -e "$1\n$2" | sort -V | head -n1`" ]
}

verlt()
{
	[ "$1" = "$2" ] && return 1 || verlte $1 $2 ]
}

GetVersion()
{
	ret="?"
	dvers=$ROOTACT/${1}
	if [ -d $dvers ]
	then
		if [ -f $dvers/Version ]
		then
			cat $dvers/Version
			return 0
		fi
	fi

	if [ $1 = "libpcap" -o $1 = "libxml2" ]
	then
		soreq=$ROOTACT/lib/${1}.so
		canon=`readlink -f $soreq`
		if [ -f $soreq -a -f $canon ]
		then
			vcan=`basename $canon`
			e="${1}.so.\(.*\)"
			v=$(expr $vcan : $e)
			echo $v
			return 0
		fi
	fi

	echo $ret
	return 1
}

DoDep()
{
	if [ ! -d $ROOTACT/$PRODUCT/apu/dependencies ]
	then
		return	0
	fi
	HRED="\033[1;31m"
	HGRE="\033[1;32m"
	LGRE="\033[0;32m"
	HBLU="\033[1;34m"
	LBLU="\033[0;34m"
	CNOR="\033[0m"
	p="package-deps"
	depversion="required"
	useversion="installed"
	printf "%-20s	%10.10s %20.20s\n" ${p} ${depversion} ${useversion}
	for d in `ls $ROOTACT/$PRODUCT/apu/dependencies/*.cfg`
	do
		depversion="-"
		useversion="?"
		depversion=$(grep "VERSION=" $d)
		depversion=$(expr $depversion : 'VERSION=\(.*\)[-].*')
		n=$(basename $d)
		p=$(expr $n : '\(.*\).cfg')
		useversion=`GetVersion $p`
		result="?"
		if	[ "$p" != "" -a "$useversion" != "?" ]
		then
			if	[ "$useversion" != "$depversion" ]
			then
				verlt "$useversion" "$depversion"
				if [ $? = 0 ]
				then
					echo -en "$HRED"
					result="!"
				else
					echo -en "$LBLU"
					result=">"
				fi
			else
				echo -en $HGRE
				result="="
			fi
		else
			echo -en "$LBLU"
			result="?"
		fi
#		echo -e "${p}\t\t${depversion}\t${useversion}"
		printf "%-20s	%10.10s %20.20s %s\n" \
			${p} ${depversion} ${useversion} ${result}
		echo -en $CNOR
	done

	return	0
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


cd $ROOTACT/$PRODUCT || ErrorExit "ROOTACT/$PRODUCT does not exist"

if [ ! -f "./Version" ]
then
	ErrorExit "./Version file not found"
fi
VERSION=`cat ./Version`

#GetVersion libpcap
#exit 0

$PROC

exit 0


