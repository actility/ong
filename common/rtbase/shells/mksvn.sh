#!/bin/bash
#-----------------------------------------------------------------------------
#
#	mksvn.sh
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
SVNCMD=""
WHERE=`pwd`

Usage()
{
	exec >&2

	echo "Usage: mksvn.sh -p product -c svncmd ..."
	echo "       mksvn.sh -p product -c svncmd ..."
	exit 1
}

Error()
{
	echo "mksvn.sh: $1" >&2
}

NotControled()
{
	echo "it seems that this directory is not under svn control"
	echo ".svn directory does not exist ..."
	exit 1
}

Controled()
{
	echo "it seems that this directory is under svn control"
	echo ".svn directory exists ..."
	exit 1
}

. $ROOTACT/rtbase/base/svn.sh

#echo "mksvn.sh $*"

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
		-c)	shift
			case $1 in
				svnstatus)
					shift
					SVNCMD=status
				;;
				svninfo)
					shift
					SVNCMD=info
				;;
				svnimport)
					shift
					SVNCMD=import
				;;
				svnnew)
					shift
					SVNCMD=new
				;;
				svncommit)
					shift
					SVNCMD=commit
				;;
				svntags|svntag)
					shift
					SVNCMD=tags
				;;
				svnls)
					shift
					SVNCMD=ls
				;;
				svnlstags)
					shift
					SVNCMD=lstags
				;;
				svndiff)
					shift
					SVNCMD=diff
				;;
				svncat)
					shift
					SVNCMD=cat
				;;
				svnadd)
					shift
					SVNCMD=add
				;;
				*)
					Error "bad svn command $1"
					Usage
				;;
			esac
		;;
		*)
			break
		;;
	esac
done

if [ -z "$PRODUCT" ]
then
	Error "product name not specified"
	Usage
fi

if [ -z "$SVNCMD" ]
then
	Error "svn command not specified"
	Usage
fi

if [ -f Version ]
then
	VERSION=`cat Version`
fi

if [ -z "$VERSION" ]
then
	Error "version not specified" >&2
	Usage
fi

echo "${SVN_ROOT_PROJECT}/${PRODUCT}"
PROJECT=${SVN_ROOT_PROJECT}/${PRODUCT}
case $SVNCMD in
	info)
		[ ! -d ./.svn ] && NotControled
		echo "+ svn $SVNCMD $*"
		svn $SVNCMD $*
		[ $? != "0" ] && echo "svn $SVNCMD error $?" && exit 1
		exit 0
	;;
	status|diff|cat)
		[ ! -d ./.svn ] && NotControled
		if [ $SVNCMD = "status" ]
		then
			echo "+ ./MAKE clean"
			./MAKE clean > /dev/null 2>&1
		fi
		echo "+ svn $SVNCMD $*"
		svn $SVNCMD $*
		[ $? != "0" ] && echo "svn $SVNCMD error $?" && exit 1
		exit 0
	;;
	ls)
		[ ! -d ./.svn ] && NotControled
		echo "+ svn $SVNCMD $*"
		svn $SVNCMD $*
		[ $? != "0" ] && echo "svn $SVNCMD error $?" && exit 1
		exit 0
	;;
	lstags)
		DIRTAGS=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TAGS}
		[ ! -d ./.svn ] && NotControled
		echo "+ svn $SVNCMD $DIRTAGS"
		svn ls $DIRTAGS
		[ $? != "0" ] && echo "svn $SVNCMD error $?" && exit 1
		exit 0
	;;
	add)
		[ ! -d ./.svn ] && NotControled
		echo "+ svn $SVNCMD"
		svn $SVNCMD --parents $*
		[ $? != "0" ] && echo "svn $SVNCMD error $?" && exit 1
		exit 0
	;;
	commit)
		[ ! -d ./.svn ] && NotControled
		echo "+	svn commit $*"
		DATE=`date`
		svn commit -m "$DATE" $*
		[ $? != "0" ] && echo "svn $SVNCMD error $?" && exit 1
		exit 0
	;;
	import|new)
		[ -d ./.svn ] && Controled
		echo "+ ./MAKE clean"
		./MAKE clean > /dev/null 2>&1
		cd ..
		if [ ! -d $PRODUCT ]
		then
			echo "directory $PRODUCT not found in $WHERE"
			exit 1
		fi
		if [ -d ${PRODUCT}-${VERSION} ]
		then
			echo "directory ${PRODUCT}-${VERSION} exists"
			exit 1
		fi
		NEWPROJECT=${SVN_ROOT_PROJECT}/${PRODUCT}
		echo "+	svn mkdir $NEWPROJECT"
		svn mkdir -m 'initial import' $NEWPROJECT
##		[ $? != "0" ] && echo "svn mkdir error $?" && exit 1
		NEWPROJECT=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TRUNK}
		echo "+	svn import -m 'initial import' $PRODUCT $NEWPROJECT"
		svn import -m 'initial import' $PRODUCT $NEWPROJECT
		[ $? != "0" ] && echo "svn import error $?" && exit 1
		if [ $SVNCMD = "import" ]
		then
			echo "svn import only; checkout is not done"
			exit 0
		fi

		mv $PRODUCT ${PRODUCT}-${VERSION}

		echo "+ svn checkout $NEWPROJECT $PRODUCT"
		svn checkout $NEWPROJECT $PRODUCT
		[ $? != "0" ] && echo "svn checkout error $?" && exit 1

		echo "you must change your current directory"

		exit 0
	;;
	svtags000)
		[ ! -d ./.svn ] && NotControled
		DATE=`date`
		PROJECT=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TRUNK}
		echo "+	svn cat $PROJECT/Version"

		echo "local version=$VERSION"
		VERSION=""
		VERSION=`svn cat $PROJECT/Version`
		[ $? != "0" ] && echo "svn cat Version $?" && exit 1
		echo "trunk version=$VERSION"

		NEWPROJECT=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TAGS}/${VERSION}
		echo "+	svn mkdir $NEWPROJECT"
		svn mkdir --parents -m "tags $DATE" $NEWPROJECT
		[ $? != "0" ] && echo "svn mkdir error $?" && exit 1

		echo "+	svn copy $PROJECT => ${PRODUCT}/${SVN_TAGS}/${VERSION}"
		svn copy -m "tags $DATE" $PROJECT $NEWPROJECT
		[ $? != "0" ] && echo "svn copy error $?" && exit 1
		echo "trunk version=$VERSION is taged now"
		exit 0
	;;
	tags)
		[ ! -d ./.svn ] && NotControled
		DATE=`date`
		PROJECT=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TRUNK}
		echo "+	svn cat $PROJECT/Version"

		echo "local version=$VERSION"
		VERSION=""
		VERSION=`svn cat $PROJECT/Version`
		[ $? != "0" ] && echo "svn cat Version $?" && exit 1
		echo "trunk version=$VERSION"

		DIRTAGS=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TAGS}
#		echo "+	svn mkdir $DIRTAGS"
		svn mkdir --parents -q -m "" $DIRTAGS > /dev/null 2>&1
#		[ $? != "0" ] && echo "svn mkdir error $?" && exit 1


		NEWTAG=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TAGS}/${PRODUCT}-${VERSION}
		svn ls ${NEWTAG} > /dev/null 2>&1
		[ $? == "0" ] && echo "$NEWTAG alreay exists $?" && exit 1

		TMPTAG=${SVN_ROOT_PROJECT}/${PRODUCT}/${SVN_TAGS}/${SVN_TRUNK}
		svn ls ${TMPTAG} > /dev/null 2>&1
		[ $? == "0" ] && echo "$TMPTAG alreay exists $?" && exit 1

		echo "+	svn copy $PROJECT => $DIRTAGS"
		svn copy -m "tags $DATE" $PROJECT $DIRTAGS
		[ $? != "0" ] && echo "svn copy error $?" && exit 1

#		echo "+	svn rename => ${DIRTAGS}/${VERSION}"
		echo "+	svn rename => ${NEWTAG}"
		svn rename -m "tags $DATE" ${DIRTAGS}/${SVN_TRUNK} ${NEWTAG}
		[ $? != "0" ] && echo "svn rename error $?" && exit 1

		echo "trunk version=$VERSION is taged now"
		exit 0
	;;


	*)
		exit 1
	;;
esac

exit 0
