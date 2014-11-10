#!/bin/sh

NB_ARGS=$#
VERSION=$1
WRKSPC_ROOT_DIR=workspace-$VERSION
REPO_DIR=repository-$VERSION
CURRENT=`pwd`
SVN_SERVER_URL="svn://10.10.12.201/"
SVN_BASE_REPO="repos/"

. $HOME/.apu/apu-tools.conf

if [ $NB_ARGS -ne 1 ]
then
  echo "Usage: $0 <ong-version>" >&2
  exit 1
fi

[ -d $WRKSPC_ROOT_DIR ] && rm -rf $WRKSPC_ROOT_DIR
mkdir -p $WRKSPC_ROOT_DIR
cd $WRKSPC_ROOT_DIR
WRKSPC_ROOT_DIR=`pwd`
cd $CURRENT

[ -d $REPO_DIR ] && rm -rf $REPO_DIR
mkdir -p $REPO_DIR
cd $REPO_DIR
REPO_DIR=`pwd`
cd $CURRENT
REPOSITORY=-Dmaven.repo.local=$REPO_DIR

  rows=3
  # first columns is the project name
  # second columns is the project sub-path under svn tree

  # APIS
  projectsApi=( \
  "java.pom" "m2m/common/pom/" "2.0.0" \
  "java.cdc.pom" "m2m/common/pom/" "3.0.0" \
  "backend.api" "m2m/ong/scl/" "3.0.0" \
  "cm.api" "m2m/ong/scl/" "1.0.0" \
  "log.api" "m2m/ong/scl/" "1.0.0" \
  "framework.resources.api" "m2m/ong/scl/" "1.0.0" \
  "storage.api" "m2m/ong/scl/" "0.2.0" \
  "storage.driver.api" "m2m/ong/scl/" "0.2.0" \
  "storage.driver.sqlite.ni.api" "m2m/ong/scl/" "2.0.0" \
  "jxo.api" "m2m/ong/scl/" "3.0.0" \
  "jxo.ni.api" "m2m/ong/scl/" "1.0.0" \
  "shutdown.hook.ni.api" "m2m/ong/scl/" "1.0.0" \
  "song.api" "m2m/ong/scl/" "4.0.0" \
  "song.binding.api" "m2m/ong/scl/" "4.0.0" \
  "song.binding.coap.ni.api" "m2m/ong/scl/" "1.0.0" \
  "song.binding.http.api" "m2m/ong/scl/" "1.0.0" \
  "song.binding.http.ni.api" "m2m/ong/scl/" "1.0.0" \
  "m2m.api" "m2m/ong/scl/" "3.0.0" \
  "transport.logger.api" "m2m/ong/scl/" "1.0.0" \
  "org.apache.felix.shell.acy.api" "m2m/ong/scl/" "1.0.0" \
  )
  nbProjectsApi=${#projectsApi[@]}

  # Impl
  projectsImpl=( \
  "util" "m2m/ong/scl/" "4.0.1" \
  "org.apache.felix.shell.acy" "m2m/ong/scl/" "2.0.0" \
  "org.apache.felix.shell.tui.acy" "m2m/ong/scl/" "1.0.0" \
  "felix.shell.telnet" "m2m/ong/scl/" "2.2.0" \
  "framework.resources.osgi" "m2m/ong/scl/" "1.0.0" \
  "cm" "m2m/ong/scl/" "1.0.3" \
  "cm.command.shell" "m2m/ong/scl/" "2.0.0" \
  "framework.command.shell.me" "m2m/ong/scl/" "2.0.0" \
  "framework.command.shell.se" "m2m/ong/scl/" "1.0.0" \
  "log" "m2m/ong/scl/" "1.0.1" \
  "log.command.shell" "m2m/ong/scl/" "2.0.1" \
  "inspector.command.shell" "m2m/ong/scl/" "1.2.0" \
  "backend" "m2m/ong/scl/" "3.0.1" \
  "backend.command.shell" "m2m/ong/scl/" "2.0.0" \
  "storage" "m2m/rms/scl/" "0.2.1" \
  "song" "m2m/ong/scl/" "4.0.4" \
  "song.binding.http" "m2m/ong/scl/" "4.0.0" \
  "song.binding.http.command.shell" "m2m/ong/scl/" "1.0.0" \
  "song.command.shell" "m2m/ong/scl/" "3.0.0" \
  "song.trace.command.shell" "m2m/ong/scl/" "3.0.0" \
  "phoneme.command.shell" "m2m/ong/scl/" "2.0.0" \
  "m2m" "m2m/ong/scl/" "3.0.4" \
  "scl.app.me" "m2m/ong/scl/" "2.0.8" \
  "system.version" "m2m/ong/scl/" "4.4.4" \
  "transport.logger.log" "m2m/ong/scl/" "1.0.1" \
  "jxo" "m2m/ong/scl/" "3.0.1" \
  "shutdown.hook" "m2m/ong/scl/" "3.0.1" \
  "song.binding.coap" "m2m/ong/scl/" "2.0.1" \
  "storage.driver.sqlite" "m2m/ong/scl/" "0.2.2" \
  )
  nbProjectsImpl=${#projectsImpl[@]}

  projectsNative=( \
  "shutdown.hook.jni" "m2m/ong/scl/" "1.0.0" \
  "jxo.jni" "m2m/ong/scl/" "1.2.1" \
  "storage.driver.sqlite.jni" "m2m/ong/scl/" "2.0.1" \
  "song.binding.coap.jni" "m2m/ong/scl/" "1.0.0" \
  "song.binding.http.jni" "m2m/ong/scl/" "1.0.1" \
  )
  nbProjectsNative=${#projectsNative[@]}

  nbProjects=$(( ($nbProjectsApi + $nbProjectsImpl + $nbProjectsNative) / 3 ))

  ###############################################################################
  ##       Checkout the sources
  ###############################################################################
  nbCurrent=1
  for i in $(seq 0 $((nbProjectsApi / 3 - 1)))
  do
    index=$(( $i * $rows ))
  PROJECT_NAME=${projectsApi[$index]}
  index=$(( $i * $rows + 1 ))
  PROJECT_SUB_PATH=${projectsApi[$index]}
  index=$(( $i * $rows + 2 ))
  PROJECT_VERSION=${projectsApi[$index]}
  cd $WRKSPC_ROOT_DIR || exit 1
  echo "#######################################################################"
  echo "checkout project: (i:$i) (name:$PROJECT_NAME) (path:$PROJECT_SUB_PATH) (version:$PROJECT_VERSION)"
  echo "progression: $nbCurrent / $nbProjects"
  echo "#######################################################################"
  svn co $SVN_SERVER_URL$SVN_BASE_REPO$PROJECT_SUB_PATH$PROJECT_NAME/tags/$PROJECT_NAME-$PROJECT_VERSION $PROJECT_NAME || exit 1
  echo ""
  cd $CURRENT
  nbCurrent=$(($nbCurrent + 1))
done

for i in $(seq 0 $((nbProjectsImpl / 3 - 1)))
do
  index=$(( $i * $rows ))
  PROJECT_NAME=${projectsImpl[$index]}
  index=$(( $i * $rows + 1 ))
  PROJECT_SUB_PATH=${projectsImpl[$index]}
  index=$(( $i * $rows + 2 ))
  PROJECT_VERSION=${projectsImpl[$index]}
  cd $WRKSPC_ROOT_DIR || exit 1
  echo "#######################################################################"
  echo "checkout project: (i:$i) (name:$PROJECT_NAME) (path:$PROJECT_SUB_PATH) (version:$PROJECT_VERSION)"
  echo "progression: $nbCurrent / $nbProjects"
  echo "#######################################################################"
  svn co $SVN_SERVER_URL$SVN_BASE_REPO$PROJECT_SUB_PATH$PROJECT_NAME/tags/$PROJECT_NAME-$PROJECT_VERSION $PROJECT_NAME || exit 1
  echo ""
  cd $CURRENT
  nbCurrent=$(($nbCurrent + 1))
done

for i in $(seq 0 $((nbProjectsNative / 3 - 1)))
do
  index=$(( $i * $rows ))
  PROJECT_NAME=${projectsNative[$index]}
  index=$(( $i * $rows + 1 ))
  PROJECT_SUB_PATH=${projectsNative[$index]}
  index=$(( $i * $rows + 2 ))
  PROJECT_VERSION=${projectsNative[$index]}
  cd $WRKSPC_ROOT_DIR || exit 1
  echo "#######################################################################"
  echo "checkout project: (i:$i) (name:$PROJECT_NAME) (path:$PROJECT_SUB_PATH) (version:$PROJECT_VERSION)"
  echo "progression: $nbCurrent / $nbProjects"
  echo "#######################################################################"
  svn co $SVN_SERVER_URL$SVN_BASE_REPO$PROJECT_SUB_PATH$PROJECT_NAME/tags/$PROJECT_NAME-$PROJECT_VERSION $PROJECT_NAME || exit 1
  echo ""
  cd $CURRENT
  nbCurrent=$(($nbCurrent + 1))
done

###############################################################################
##       retrieve the dependencies
###############################################################################
nbCurrent=1
for i in $(seq 0 $((nbProjectsApi / 3 - 1)))
do
  index=$(( $i * $rows ))
  PROJECT_NAME=${projectsApi[$index]}
  cd $WRKSPC_ROOT_DIR || exit 1
  echo "#######################################################################"
  echo "Compile project: (i:$i) (name:$PROJECT_NAME)"
  echo "progression: $nbCurrent / $nbProjects"
  echo "#######################################################################"
  cd $PROJECT_NAME || exit 1
  mvn $REPOSITORY clean install || exit 1
  mvn $REPOSITORY eclipse:eclipse -DdownloadSources=true  -DdownloadJavadocs=true || exit 1
  echo ""
  cd $CURRENT
  nbCurrent=$(($nbCurrent + 1))
done

for i in $(seq 0 $((nbProjectsImpl / 3 - 1)))
do
  index=$(( $i * $rows ))
  PROJECT_NAME=${projectsImpl[$index]}
  cd $WRKSPC_ROOT_DIR || exit 1
  echo "#######################################################################"
  echo "Compile project: (i:$i) (name:$PROJECT_NAME)"
  echo "progression: $nbCurrent / $nbProjects"
  echo "#######################################################################"
  cd $PROJECT_NAME || exit 1
  mvn $REPOSITORY clean install || exit 1
  mvn $REPOSITORY eclipse:eclipse -DdownloadSources=true  -DdownloadJavadocs=true || exit 1
  echo ""
  cd $CURRENT
  nbCurrent=$(($nbCurrent + 1))
done

for i in $(seq 0 $((nbProjectsNative / 3 - 1)))
do
  index=$(( $i * $rows ))
  PROJECT_NAME=${projectsNative[$index]}
  cd $WRKSPC_ROOT_DIR || exit 1
  echo "#######################################################################"
  echo "Compile project: (i:$i) (name:$PROJECT_NAME)"
  echo "progression: $nbCurrent / $nbProjects"
  echo "#######################################################################"
  cd $PROJECT_NAME || exit 1
  mvn $REPOSITORY clean install -Dbuild.type=$HOST_TARGET || exit 1
  mvn $REPOSITORY eclipse:eclipse -DdownloadSources=true  -DdownloadJavadocs=true -Dbuild.type=$HOST_TARGET || exit 1
  echo ""
  cd $CURRENT
  nbCurrent=$(($nbCurrent + 1))
done

###############################################################################

echo "#######################################################################"
echo "  Now packing the boot repository"
echo "#######################################################################"
cd $REPO_DIR
# clean-up artifacts
rm -f $(find . -name _maven.repositories)
rm -f $(find . -name *.lastUpdated)

tar zcvf $CURRENT/boot-repository-$VERSION.tgz *
cd $CURRENT
rm -rf $REPO_DIR $WRKSPC_ROOT_DIR


echo "#######################################################################"
echo "  Completed successfully !"
echo "#######################################################################"

