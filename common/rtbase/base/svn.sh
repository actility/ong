
#
#	svn server 
#
SVN_URL="svn://10.10.12.201"
SVN_REPOS="repos"
SVN_TRUNK="trunk"
SVN_TAGS="tags"

#
#	path for projects
#
SVN_DEF_ROOT_PROJECT="m2m/ong/drivers/common"
#adjust SVN_DEF_ROOT_PROJECT
if [ -f svn/svn.sh ]
then
	. svn/svn.sh
fi

#
#
#
SVN_ROOT_PROJECT=${SVN_URL}/${SVN_REPOS}/${SVN_DEF_ROOT_PROJECT}
