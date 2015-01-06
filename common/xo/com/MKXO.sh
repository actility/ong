
help=""
upper=""
verbose=""
gener=""
file=""
preserve="0"

case	$# in
	0)
		echo	"MKXO [-p] [-U] [-h] [-v] [-g] filedef"
		exit	1
	;;
	*)
		while	[ $# -gt 0 ]
		do
			case	$1 in
				-U)
					shift
					upper="-U"
				;;
				-h)
					shift
					help="-h"
				;;
				-v)
					shift
					verbose="-v"
				;;
				-g)
					shift
					gener="-g"
				;;
				-p)
					shift
					preserve="1"
				;;
				*)
					file=$1
					shift
				;;
			esac
		done
	;;
esac

#echo	$help $upper $verbose $gener $file

if	[ ! -f $file.ref ]
then
	echo	"file : $file.ref ???"
	exit	1
fi


if [ "$OSCLASS" = "WIN32" ]
then
	cl	-E $file.ref > $file.$$.ref
else
	cp $file.ref $file.$$.c
	cc -E $file.$$.c > $file.$$.ref
	if [ "$preserve" = "1" ]
	then
		cp $file.$$.ref $file.i
	fi
	rm $file.$$.c
fi

if [ "$EXESFX" = "" ]
then
	EXESFX="x"
fi

if	[ -f $file.$$.ref ]
then
	${ROOTACT}/xo/com/mkxo.${EXESFX} $help $upper $verbose $gener -f $file.$$
else
	echo	"preprocessor error"
	exit	1
fi

if	[ -f $file.$$.ref ]
then
	rm $file.$$.ref
fi
if	[ -f $file.$$.xor ]
then
	mv $file.$$.xor $file.xor
fi
if	[ -f def$file.$$.h ]
then
	mv def$file.$$.h def$file.h
fi
if	[ -f gen$file.$$.c ]
then
	mv gen$file.$$.c gen$file.c
fi
