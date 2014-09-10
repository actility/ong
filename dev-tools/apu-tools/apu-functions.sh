#!/bin/bash

#
# Copyright   Actility, SA. All Rights Reserved.
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License version
# 2 only, as published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License version 2 for more details (a copy is
# included at /legal/license.txt).
# 
# You should have received a copy of the GNU General Public License
# version 2 along with this work; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA
# 
# Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
# or visit www.actility.com if you need additional
# information or have any questions.
# 
# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$
#

AFN_APU_TOOLS_VERS=2.0.0

# Define Local Configuration
AFN_MVN_LOCAL_REPO=~/.m2/repository

# Define Nexus Configuration
AFN_REST_PATH=/service/local
AFN_ART_REDIR=/artifact/maven/redirect
AFN_READ_PATH=/repositories/releases/content
AFN_DEPLOY_PATH=/content/repositories/releases/

AFN_REDIRECT_URL=${NEXUS_BASE}${AFN_REST_PATH}${AFN_ART_REDIR}
AFN_DEPLOY_URL=${NEXUS_BASE}${AFN_DEPLOY_PATH}
AFN_READ_URL=${NEXUS_BASE}${AFN_REST_PATH}${AFN_READ_PATH}

AFN_TEMP_DIR=.apu_tmp
AFN_DEPENDENCIES_SUB_DIR=apu/dependencies

AFN_TAR_EXCLUDE_PATTERN="--exclude-vcs"
TAR_EXCLUDE_PATTERN="--exclude-vcs"

AFN_DEPLOY=0
AFN_CLEAN=0

functionExists()
{
  declare -F $1 > /dev/null
  return $?
}

fetchDependency()
{
  _AFN_DEP_FILE=$1
  _AFN_TARGET_NAME=$2

  if ! [ -e "$_AFN_DEP_FILE" ]
  then
    echo "fetchDependency: no valid DEP_FILE provided ($_AFN_DEP_FILE)" >&2
    return 1
  fi

  if [ -z "$_AFN_TARGET_NAME" ]
  then
    echo "fetchDependency: no valid TARGET_NAME provided ($_AFN_TARGET_NAME)" >&2
    return 1
  fi

  unset DEPENDENCY_TYPE
  unset GROUP_ID
  unset ARTIFACT_ID
  unset VERSION
  unset REPO
  unset PACKAGING
  unset TARGET_DIR
  unset WITH_ARCH
  unset INCLUDE_ARCH

  # source the dependency file (first param)
  . $_AFN_DEP_FILE

  # The classifier is the target name provided as parameter
  CLASSIFIER=$_AFN_TARGET_NAME

  # check if this dependency is needed for this targeted arch
  if [ -n "$INCLUDE_ARCH" ] && \
     [ "$INCLUDE_ARCH" != "*" ] && \
     [ -z "$(echo $INCLUDE_ARCH | grep $_AFN_TARGET_NAME)" ]
  then
    echo "skipped... dependency not included for this arch."
    return 0
  fi

  # WITH_ARCH=2 means that the architecture is to be forced to the architecture of the
  # machine used for compilation
  if [ "x2" == "x$WITH_ARCH" ]
  then
    CLASSIFIER=$HOST_TARGET
  elif [ "x0" == "x$WITH_ARCH" ]
  then
    CLASSIFIER="none"
  fi

  _AFN_APU_FILE=$ARTIFACT_ID-$VERSION-$CLASSIFIER.$PACKAGING
  if [ "x0" == "x$WITH_ARCH" ]
  then
    _AFN_APU_FILE=$ARTIFACT_ID-$VERSION.$PACKAGING
  fi

  _AFN_LOCAL_DIR=$AFN_MVN_LOCAL_REPO/$(echo $GROUP_ID | sed -e "s/\./\//g")/$ARTIFACT_ID/$VERSION

  if [ ! -f $_AFN_LOCAL_DIR/$_AFN_APU_FILE ]
  then
    echo -n "($CLASSIFIER) (remote) "
    if [ "x0" == "x$WITH_ARCH" ]
    then
      mvnGetDependencyNoArch $GROUP_ID $ARTIFACT_ID $VERSION $PACKAGING || return 1
    else
      mvnGetDependency $GROUP_ID $ARTIFACT_ID $VERSION $PACKAGING $CLASSIFIER || return 1
    fi
  else
    echo -n "($CLASSIFIER) (local) "
  fi
}

fetchAndInstallDependency()
{
  _AFN_DEP_FILE=$1
  _AFN_TARGET_NAME=$2
  _AFN_BUILD_DIR=$3

  if ! [ -e "$_AFN_DEP_FILE" ]
  then
    echo "fetchAndInstallDependency: no valid DEP_FILE provided ($_AFN_DEP_FILE)" >&2
    return 1
  fi

  if [ -z "$_AFN_TARGET_NAME" ]
  then
    echo "fetchAndInstallDependency: no valid TARGET_NAME provided ($_AFN_TARGET_NAME)" >&2
    return 1
  fi

  if ! [ -d "$_AFN_BUILD_DIR" ]
  then
    echo "fetchAndInstallDependency: no valid _BUILD_DIR provided ($_AFN_BUILD_DIR)" >&2
    return 1
  fi

  unset DEPENDENCY_TYPE
  unset GROUP_ID
  unset ARTIFACT_ID
  unset VERSION
  unset REPO
  unset PACKAGING
  unset TARGET_DIR
  unset WITH_ARCH
  unset INCLUDE_ARCH
  
  # source the dependency file (first param)
  . $_AFN_DEP_FILE

  # The classifier is the target name provided as parameter
  CLASSIFIER=$_AFN_TARGET_NAME

  # check if this dependency is needed for this targeted arch
  if [ -n "$INCLUDE_ARCH" ] && \
     [ "$INCLUDE_ARCH" != "*" ] && \
     [ -z "$(echo $INCLUDE_ARCH | grep $_AFN_TARGET_NAME)" ]
  then
    echo "skipped... dependency not included for this arch."
    return 0
  fi

  # WITH_ARCH=2 means that the architecture is to be forced to the architecture of the
  # machine used for compilation
  if [ "x2" == "x$WITH_ARCH" ]
  then
    CLASSIFIER=$HOST_TARGET
  elif [ "x0" == "x$WITH_ARCH" ]
  then
    CLASSIFIER="none"
  fi

  _AFN_APU_FILE=$ARTIFACT_ID-$VERSION-$CLASSIFIER.$PACKAGING
  if [ "x0" == "x$WITH_ARCH" ]
  then
    _AFN_APU_FILE=$ARTIFACT_ID-$VERSION.$PACKAGING
  fi

  _AFN_LOCAL_DIR=$AFN_MVN_LOCAL_REPO/$(echo $GROUP_ID | sed -e "s/\./\//g")/$ARTIFACT_ID/$VERSION

  if [ ! -f $_AFN_LOCAL_DIR/$_AFN_APU_FILE ]
  then
    echo -n "($CLASSIFIER) (remote) "
    if [ "x0" == "x$WITH_ARCH" ]
    then
      mvnGetDependencyNoArch $GROUP_ID $ARTIFACT_ID $VERSION $PACKAGING || return 1
    else
      mvnGetDependency $GROUP_ID $ARTIFACT_ID $VERSION $PACKAGING $CLASSIFIER || return 1
    fi
    #fetchNexusDependency $_AFN_LOCAL_DIR $_AFN_APU_FILE $_AFN_BUILD_DIR
  else
    echo -n "($CLASSIFIER) (local) "
  fi

  installPackage $PACKAGING $_AFN_LOCAL_DIR/$_AFN_APU_FILE $_AFN_BUILD_DIR
}

lsTargets()
{
  _AFN_TARGET_GROUP_ID=com.actility.m2m
  if [ -n "$1" ]; then
    _AFN_TARGET_GROUP_ID=$1
  fi
  _AFN_TARGET_PARENT=~/.apu/targets/$(echo $_AFN_TARGET_GROUP_ID | sed -e "s/\./\//g")/
  _AFN_RESULTS=$(ls $_AFN_TARGET_PARENT)
  for _AFN_RESULT in $_AFN_RESULTS
  do
    _AFN_TEST=$(echo $_AFN_RESULT | sed -n "s/^target-\(.*\)$/\1/p")
    if [ -n "$_AFN_TEST" ]; then
      echo $_AFN_TEST
    fi
  done
}

list()
{
  _AFN_PATH=$1

  _AFN_LIST_FILE=$AFN_TEMP_DIR/list.xml
  mkdir -p $AFN_TEMP_DIR

  _AFN_READ_URL=${AFN_READ_URL}$_AFN_PATH/?isLocal
  curl -sS -L -o $_AFN_LIST_FILE $_AFN_READ_URL || exitWithMsg "Unknown path $_AFN_PATH." 1
  _AFN_TEST=$(cat $_AFN_LIST_FILE | grep "^<content")

  if [ -n "$_AFN_TEST" ]
  then
    _AFN_ELEMS=$(echo cat "/content/data/content-item/text/text()" | xmllint --nocdata --shell $_AFN_LIST_FILE | awk '{if(NR%2!=1)print}')
    rm -rf $AFN_TEMP_DIR
    echo $_AFN_ELEMS | tr -s ' ' '\n'
  else
    exitWithMsg "Unknown path $_AFN_PATH." 1
  fi
}

checkApuMinVersion()
{
  _AFN_EXP_VERION=$1

  _AFN_EXP_MAJOR=$(echo -n $_AFN_EXP_VERION | cut -d'.' -f1)
  _AFN_EXP_MINOR=$(echo -n $_AFN_EXP_VERION | cut -d'.' -f2)
  _AFN_EXP_BUG=$(echo -n $_AFN_EXP_VERION | cut -d'.' -f3)

  _AFN_APU_MAJOR=$(echo -n $AFN_APU_TOOLS_VERS | cut -d'.' -f1)
  _AFN_APU_MINOR=$(echo -n $AFN_APU_TOOLS_VERS | cut -d'.' -f2)
  _AFN_APU_BUG=$(echo -n $AFN_APU_TOOLS_VERS | cut -d'.' -f3)

  if [ "$_AFN_APU_MAJOR" -lt "$_AFN_EXP_MAJOR" ]; then
    exitWithMsg "Bad apu-tools version ($AFN_APU_TOOLS_VERS) expected minimum version ($_AFN_EXP_VERION)." 1
  elif [ "$_AFN_APU_MAJOR" == "$_AFN_EXP_MAJOR" ]; then
    if [ "$_AFN_APU_MINOR" -lt "$_AFN_EXP_MINOR" ]; then
      exitWithMsg "Bad apu-tools version ($AFN_APU_TOOLS_VERS) expected minimum version ($_AFN_EXP_VERION)." 1
    elif [ "$_AFN_APU_MINOR" == "$_AFN_EXP_MINOR" ]; then
      if [ "$_AFN_APU_BUG" -lt "$_AFN_EXP_BUG" ]; then
        exitWithMsg "Bad apu-tools version ($AFN_APU_TOOLS_VERS) expected minimum version ($_AFN_EXP_VERION)." 1
      fi
    fi
  fi
}

checkApuMaxVersion()
{
  _AFN_EXP_VERION=$1

  _AFN_EXP_MAJOR=$(echo -n $_AFN_EXP_VERION | cut -d'.' -f1)
  _AFN_EXP_MINOR=$(echo -n $_AFN_EXP_VERION | cut -d'.' -f2)
  _AFN_EXP_BUG=$(echo -n $_AFN_EXP_VERION | cut -d'.' -f3)

  _AFN_APU_MAJOR=$(echo -n $AFN_APU_TOOLS_VERS | cut -d'.' -f1)
  _AFN_APU_MINOR=$(echo -n $AFN_APU_TOOLS_VERS | cut -d'.' -f2)
  _AFN_APU_BUG=$(echo -n $AFN_APU_TOOLS_VERS | cut -d'.' -f3)

  if [ "$_AFN_APU_MAJOR" -gt "$_AFN_EXP_MAJOR" ]; then
    exitWithMsg "Bad apu-tools version ($AFN_APU_TOOLS_VERS) expected maximum version ($_AFN_EXP_VERION)." 1
  elif [ "$_AFN_APU_MAJOR" == "$_AFN_EXP_MAJOR" ]; then
    if [ "$_AFN_APU_MINOR" -gt "$_AFN_EXP_MINOR" ]; then
      exitWithMsg "Bad apu-tools version ($AFN_APU_TOOLS_VERS) expected maximum version ($_AFN_EXP_VERION)." 1
    elif [ "$_AFN_APU_MINOR" == "$_AFN_EXP_MINOR" ]; then
      if [ "$_AFN_APU_BUG" -gt "$_AFN_EXP_BUG" ]; then
        exitWithMsg "Bad apu-tools version ($AFN_APU_TOOLS_VERS) expected maximum version ($_AFN_EXP_VERION)." 1
      fi
    fi
  fi
}

sortVersions()
{
  _AFN_VERSIONS=$*
  _AFN_VERSIONS_ACC=""

  for _AFN_VERSION in $_AFN_VERSIONS
  do
    _AFN_PATTERN1=$(echo $_AFN_VERSION | sed -n "/^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*-.*$/p")
    _AFN_PATTERN2=$(echo $_AFN_VERSION | sed -n "/^[0-9][0-9]*\.[0-9][0-9]*-.*$/p")
    _AFN_PATTERN3=$(echo $_AFN_VERSION | sed -n "/^[0-9][0-9]*-.*$/p")
    _AFN_PATTERN4=$(echo $_AFN_VERSION | sed -n "/^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$/p")
    _AFN_PATTERN5=$(echo $_AFN_VERSION | sed -n "/^[0-9][0-9]*\.[0-9][0-9]*$/p")
    _AFN_PATTERN6=$(echo $_AFN_VERSION | sed -n "/^[0-9][0-9]*$/p")
    if [ -n "$_AFN_PATTERN1" ]
    then
      _AFN_MAJOR=$(echo $_AFN_VERSION | cut -d. -f1)
      _AFN_MINOR=$(echo $_AFN_VERSION | cut -d. -f2)
      _AFN_INC=$(echo $_AFN_VERSION | cut -d. -f3 | cut -d- -f1)
      _AFN_QUALIFIER="_$(echo $_AFN_VERSION | cut -d- -f2)"
      _AFN_VERSIONS_ACC="$_AFN_MAJOR $_AFN_MINOR $_AFN_INC $_AFN_QUALIFIER $_AFN_VERSION|$_AFN_VERSIONS_ACC"
    elif [ -n "$_AFN_PATTERN2" ]
    then
      _AFN_MAJOR=$(echo $_AFN_VERSION | cut -d. -f1)
      _AFN_MINOR=$(echo $_AFN_VERSION | cut -d. -f2 | cut -d- -f1)
      _AFN_INC="0"
      _AFN_QUALIFIER="_$(echo $_AFN_VERSION | cut -d- -f2)"
      _AFN_VERSIONS_ACC="$_AFN_MAJOR $_AFN_MINOR $_AFN_INC $_AFN_QUALIFIER $_AFN_VERSION|$_AFN_VERSIONS_ACC"
    elif [ -n "$_AFN_PATTERN3" ]
    then
      _AFN_MAJOR=$(echo $_AFN_VERSION | cut -d- -f1)
      _AFN_MINOR="0"
      _AFN_INC="0"
      _AFN_QUALIFIER="_$(echo $_AFN_VERSION | cut -d- -f2)"
      _AFN_VERSIONS_ACC="$_AFN_MAJOR $_AFN_MINOR $_AFN_INC $_AFN_QUALIFIER $_AFN_VERSION|$_AFN_VERSIONS_ACC"
    elif [ -n "$_AFN_PATTERN4" ]
    then
      _AFN_MAJOR=$(echo $_AFN_VERSION | cut -d. -f1)
      _AFN_MINOR=$(echo $_AFN_VERSION | cut -d. -f2)
      _AFN_INC=$(echo $_AFN_VERSION | cut -d. -f3)
      _AFN_QUALIFIER="_"
      _AFN_VERSIONS_ACC="$_AFN_MAJOR $_AFN_MINOR $_AFN_INC $_AFN_QUALIFIER $_AFN_VERSION|$_AFN_VERSIONS_ACC"
    elif [ -n "$_AFN_PATTERN5" ]
    then
      _AFN_MAJOR=$(echo $_AFN_VERSION | cut -d. -f1)
      _AFN_MINOR=$(echo $_AFN_VERSION | cut -d. -f2)
      _AFN_INC="0"
      _AFN_QUALIFIER="_"
      _AFN_VERSIONS_ACC="$_AFN_MAJOR $_AFN_MINOR $_AFN_INC $_AFN_QUALIFIER $_AFN_VERSION|$_AFN_VERSIONS_ACC"
    elif [ -n "$_AFN_PATTERN6" ]
    then
      _AFN_MAJOR=$(echo $_AFN_VERSION)
      _AFN_MINOR="0"
      _AFN_INC="0"
      _AFN_QUALIFIER="_"
      _AFN_VERSIONS_ACC="$_AFN_MAJOR $_AFN_MINOR $_AFN_INC $_AFN_QUALIFIER $_AFN_VERSION|$_AFN_VERSIONS_ACC"
    fi
  done

  echo $_AFN_VERSIONS_ACC | tr -s '|' '\n' | sort -nr -k1,1 -k2,2 -k3,3 | cut -d" " -f5 | tr -s '\n' ' '
}

listVersions()
{
  _AFN_GROUP_ID=$1
  _AFN_ARTIFACT_ID=$2

  _AFN_VERSIONS_FILE=$AFN_TEMP_DIR/versions.xml
  mkdir -p $AFN_TEMP_DIR
  
  _AFN_READ_URL=${AFN_READ_URL}/$(echo $_AFN_GROUP_ID | sed -e "s/\./\//g")/$_AFN_ARTIFACT_ID/?isLocalFN_
  curl -sS -L -o $_AFN_VERSIONS_FILE $_AFN_READ_URL || exitWithMsg "Unknown project $_AFN_GROUP_ID:$_AFN_ARTIFACT_ID." 1 
  _AFN_TEST=$(cat $_AFN_VERSIONS_FILE | grep "^<content")

  if [ -n "$_AFN_TEST" ]
  then
    sortVersions $(echo cat "/content/data/content-item/text/text()" | xmllint --nocdata --shell $_AFN_VERSIONS_FILE | awk '{if(NR%2!=1)print}') | tr -s ' ' '\n'
    rm -rf $AFN_TEMP_DIR
  else
    exitWithMsg "Unknown project $_AFN_GROUP_ID:$_AFN_ARTIFACT_ID." 1
  fi
}

fetchNexusDependency()
{
  _AFN_LOCAL_DIR=$1
  _AFN_APU_FILE=$2
  _AFN_BUILD_DIR=$3

  # build the URL to retrieve the dependency
  _AFN_PARAM_KEYS=( g a v r p c )
  _AFN_PARAM_VALUES=( $GROUP_ID $ARTIFACT_ID $VERSION $REPO $PACKAGING $CLASSIFIER )

  if [ "x0" == "x$WITH_ARCH" ]
  then
    _AFN_PARAM_KEYS=( g a v r p )
    _AFN_PARAM_VALUES=( $GROUP_ID $ARTIFACT_ID $VERSION $REPO $PACKAGING )
  fi

  _AFN_PARAMS=""
  for _AFN_INDEX in ${!_AFN_PARAM_KEYS[*]} 
  do
    if [[ ${_AFN_PARAM_VALUES[$_AFN_INDEX]} != "" ]]
    then
      _AFN_PARAMS="${_AFN_PARAMS}${_AFN_PARAM_KEYS[$_AFN_INDEX]}=${_AFN_PARAM_VALUES[$_AFN_INDEX]}&"
    else
      echo "oops... parameter ${_AFN_PARAM_KEYS[$_AFN_INDEX]} is missing. Please check $_AFN_LOCAL_DIR content" >&2
      return 1 
    fi
  done

  mkdir -p $_AFN_LOCAL_DIR
  mkdir -p $_AFN_BUILD_DIR
  cd $_AFN_BUILD_DIR

  _AFN_REDIRECT_URL="${AFN_REDIRECT_URL}?${_AFN_PARAMS}"
  curl -sS -L -o $_AFN_LOCAL_DIR/$_AFN_APU_FILE $_AFN_REDIRECT_URL

  if [[ $(head -n 1 $_AFN_LOCAL_DIR/$_AFN_APU_FILE) =~ "<html>" ]]
  then
    rm -rf $_AFN_LOCAL_DIR/$_AFN_APU_FILE
    echo "Unable to retrieve apu $_AFN_APU_FILE" >&2
    return 1
  fi

  cp $_AFN_LOCAL_DIR/$_AFN_APU_FILE $_AFN_BUILD_DIR/$_AFN_APU_FILE
}

installPackage()
{
  _AFN_PACKAGING=$1
  _AFN_FILE_PATH=$2
  _AFN_BUILD_DIR=$3

  # unpack the apu file
  if [ "xapu" = "x$PACKAGING" ]; then
    mkdir -p $_AFN_BUILD_DIR/$AFN_TEMP_DIR || return 1
    tar zxf $_AFN_FILE_PATH -C $_AFN_BUILD_DIR/$AFN_TEMP_DIR > /dev/null || return 1

    if [ ! -f $_AFN_BUILD_DIR/$AFN_TEMP_DIR/control.tar.gz -o ! -f $_AFN_BUILD_DIR/$AFN_TEMP_DIR/data.tar.gz ]; then
      echo "Unable to find control.tar.gz and data.tar.gz files in $_AFN_FILE_PATH" >&2
    fi

    tar zxf $_AFN_BUILD_DIR/$AFN_TEMP_DIR/control.tar.gz -C $_AFN_BUILD_DIR/$AFN_TEMP_DIR > /dev/null || return 1

    # preinst
    if [ -f $_AFN_BUILD_DIR/$AFN_TEMP_DIR/preinst ]
    then
      chmod +x $_AFN_BUILD_DIR/$AFN_TEMP_DIR/preinst
      (
        export ROOTACT=$_AFN_BUILD_DIR/ 
        $_AFN_BUILD_DIR/$AFN_TEMP_DIR/preinst
      )
    fi

    # install
    tar zxf $_AFN_BUILD_DIR/$AFN_TEMP_DIR/data.tar.gz -C $_AFN_BUILD_DIR || return 1

    # postinst  
    if [ -f $_AFN_BUILD_DIR/$AFN_TEMP_DIR/postinst ]
    then
      chmod +x $_AFN_BUILD_DIR/$AFN_TEMP_DIR/postinst
      (
        export ROOTACT=$_AFN_BUILD_DIR/ 
        $_AFN_BUILD_DIR/$AFN_TEMP_DIR/postinst
      )
    fi
   
    rm -rf $_AFN_BUILD_DIR/$AFN_TEMP_DIR
  elif [ "xtgz" = "x$PACKAGING" ]; then
    tar zxf $_AFN_FILE_PATH -C $_AFN_BUILD_DIR > /dev/null || return 1
  else
    cp $_AFN_FILE_PATH $_AFN_BUILD_DIR
  fi
}

exitWithMsg() {
  echo $1 >&2
  exit $2
}

applyPatch() {
  echo -n "Apply $1..."
  patch -p1 -i $1 || { cd - > /dev/null; echo "FAILED"; exit 1; }
  echo "OK"
}

buildApu() {
  _AFN_PACKAGE=$1
  _AFN_VERSION=$2
  _AFN_APUREV=$3
  _AFN_ARCH=$4
  _AFN_DESCRIPTION=$5
  _AFN_DEST_REPO=$6
  
  echo -n "Building APU for $_AFN_PACKAGE..."
  _AFN_SIZE=$(du -sk apu/data | sed -e "s|\([0-9]*\).*|\1|")

  echo "Package: $_AFN_PACKAGE" > apu/control/control
  echo "Installed-Size: $_AFN_SIZE" >> apu/control/control
  echo "Filename: ./$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV-$_AFN_ARCH.apu" >> apu/control/control
  echo "Version: $_AFN_VERSION" >> apu/control/control
  echo "Description: $_AFN_DESCRIPTION" >> apu/control/control

  cd apu/data
  tar $AFN_TAR_EXCLUDE_PATTERN -czf ../data.tar.gz *
  cd ../control
  tar $AFN_TAR_EXCLUDE_PATTERN -czf ../control.tar.gz *
  cd ..
  tar czf ../$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV-$_AFN_ARCH.apu data.tar.gz control.tar.gz
  cd ..

  [ -n "$_AFN_DEST_REPO" ] && cp -f $_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV-$_AFN_ARCH.apu $_AFN_DEST_REPO/

  echo "OK"
}

buildApuNoArch() {
  _AFN_PACKAGE=$1
  _AFN_VERSION=$2
  _AFN_APUREV=$3
  _AFN_DESCRIPTION=$4
  _AFN_DEST_REPO=$5
  
  echo -n "Building APU for $_AFN_PACKAGE..."
  _AFN_SIZE=$(du -sk apu/data | sed -e "s|\([0-9]*\).*|\1|")
  _AFN_DESCRIPTION=$(projectDescription)

  echo "Package: $_AFN_PACKAGE" > apu/control/control
  echo "Installed-Size: $_AFN_SIZE" >> apu/control/control
  echo "Filename: ./$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV.apu" >> apu/control/control
  echo "Version: $_AFN_VERSION" >> apu/control/control
  echo "Description: $_AFN_DESCRIPTION" >> apu/control/control

  cd apu/data
  tar $AFN_TAR_EXCLUDE_PATTERN -czf ../data.tar.gz *
  cd ../control
  tar $AFN_TAR_EXCLUDE_PATTERN -czf ../control.tar.gz *
  cd ..
  tar czf ../$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV.apu data.tar.gz control.tar.gz
  cd ..

  [ -n "$_AFN_DEST_REPO" ] && cp -f $_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV.apu $_AFN_DEST_REPO/

  echo "OK"
}

mvnGetDependency() {
  _AFN_GROUP=$1
  _AFN_PACKAGE=$2
  _AFN_VERSION=$3
  _AFN_PACKAGING=$4
  _AFN_ARCH=$5

  mvn org.apache.maven.plugins:maven-dependency-plugin:2.8:get \
    -DremoteRepositories=$AFN_DEPLOY_URL -DgroupId=$_AFN_GROUP -DartifactId=$_AFN_PACKAGE \
    -Dversion=$_AFN_VERSION -Dclassifier=$_AFN_ARCH -Dpackaging=$_AFN_PACKAGING 2>&1 > /dev/null
}

mvnGetDependencyNoArch() {
  _AFN_GROUP=$1
  _AFN_PACKAGE=$2
  _AFN_VERSION=$3
  _AFN_PACKAGING=$4

  mvn org.apache.maven.plugins:maven-dependency-plugin:2.8:get \
    -DremoteRepositories=$AFN_DEPLOY_URL -DgroupId=$_AFN_GROUP -DartifactId=$_AFN_PACKAGE \
    -Dversion=$_AFN_VERSION -Dpackaging=$_AFN_PACKAGING 2>&1 > /dev/null
}

mvnInstall() {
  _AFN_GROUP=$1
  _AFN_PACKAGE=$2
  _AFN_VERSION=$3
  _AFN_APUREV=$4
  _AFN_ARCH=$5

  mvn install:install-file -DgroupId=$_AFN_GROUP -DartifactId=$_AFN_PACKAGE -Dversion=$_AFN_VERSION-$_AFN_APUREV \
    -Dclassifier=$_AFN_ARCH -Dpackaging=apu -Dfile=$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV-$_AFN_ARCH.apu \
    -DgeneratePom=true
}

mvnInstallNoArch() {
  _AFN_GROUP=$1
  _AFN_PACKAGE=$2
  _AFN_VERSION=$3
  _AFN_APUREV=$4

  mvn install:install-file -DgroupId=$_AFN_GROUP -DartifactId=$_AFN_PACKAGE -Dversion=$_AFN_VERSION-$_AFN_APUREV \
    -Dpackaging=apu -Dfile=$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV.apu -DgeneratePom=true
}

mvnDeploy() {
  _AFN_GROUP=$1
  _AFN_PACKAGE=$2
  _AFN_VERSION=$3
  _AFN_APUREV=$4
  _AFN_ARCH=$5
  
  mvn deploy:deploy-file -DgroupId=$_AFN_GROUP -DartifactId=$_AFN_PACKAGE -Dversion=$_AFN_VERSION-$_AFN_APUREV \
    -Dclassifier=$_AFN_ARCH -Dpackaging=apu -Dfile=$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV-$_AFN_ARCH.apu \
    -DrepositoryId=central -Durl=$AFN_DEPLOY_URL -DgeneratePom=true
}

mvnDeployNoArch() {
  _AFN_GROUP=$1
  _AFN_PACKAGE=$2
  _AFN_VERSION=$3
  _AFN_APUREV=$4
  
  mvn deploy:deploy-file -DgroupId=$_AFN_GROUP -DartifactId=$_AFN_PACKAGE -Dversion=$_AFN_VERSION-$_AFN_APUREV \
    -Dpackaging=apu -Dfile=$_AFN_PACKAGE-$_AFN_VERSION-$_AFN_APUREV.apu -DrepositoryId=central \
     -Durl=$AFN_DEPLOY_URL -DgeneratePom=true
}

installTarget()
{
  _AFN_TARGET=$1
  # Parse TARGET
  _AFN_TARGET_GROUP_ID=com.actility.m2m
  _AFN_TARGET_VERSION=

  i=0
  for _AFN_TOKEN in $(echo $_AFN_TARGET | tr -s ":" "\n")
  do
    _AFN_TOKENS[$i]=$_AFN_TOKEN
    i=$(expr $i + 1)
  done

  if [ "x${#_AFN_TOKENS[@]}" = "x1" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-cross-${_AFN_TOKENS[0]}
    _AFN_TARGET=${_AFN_TOKENS[0]}
  elif [ "x${#_AFN_TOKENS[@]}" = "x2" ]; then
    _AFN_TARGET_GROUP_ID=${_AFN_TOKENS[0]}
    _AFN_TARGET_ARTIFACT_ID=target-cross-${_AFN_TOKENS[1]}
    _AFN_TARGET=${_AFN_TOKENS[1]}
  else
    _AFN_TARGET_GROUP_ID=${_AFN_TOKENS[0]}
    _AFN_TARGET_ARTIFACT_ID=target-cross-${_AFN_TOKENS[1]}
    _AFN_TARGET_VERSION=${_AFN_TOKENS[2]}
    _AFN_TARGET=${_AFN_TOKENS[1]}
  fi

  if [ "$_AFN_TARGET" = "$HOST_TARGET" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-$HOST_TARGET
  elif [ "$_AFN_TARGET" = "$HOST_ARCH" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-$HOST_ARCH
  fi

  if [ "$_AFN_TARGET" = "$HOST_TARGET" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-$HOST_TARGET
  elif [ "$_AFN_TARGET" = "$HOST_ARCH" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-$HOST_ARCH
  fi

  _AFN_TARGET_PARENT=~/.apu/targets/$(echo $_AFN_TARGET_GROUP_ID | sed -e "s/\./\//g")/$_AFN_TARGET_ARTIFACT_ID

  if [ -z "$_AFN_TARGET_VERSION" ]; then
    echo "Get latest version of $_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID"
    # Get the latest version
    _AFN_LOCAL_TARGET_PARENT_DIR=$AFN_MVN_LOCAL_REPO/$(echo $_AFN_TARGET_GROUP_ID | sed -e "s/\./\//g")/$_AFN_TARGET_ARTIFACT_ID
    mkdir -p $AFN_TEMP_DIR
    echo "DEPENDENCY_TYPE=repository" > $AFN_TEMP_DIR/target.cfg
    echo "GROUP_ID=$_AFN_TARGET_GROUP_ID" >> $AFN_TEMP_DIR/target.cfg
    echo "ARTIFACT_ID=$_AFN_TARGET_ARTIFACT_ID" >> $AFN_TEMP_DIR/target.cfg
    echo "VERSION=LATEST" >> $AFN_TEMP_DIR/target.cfg
    echo "REPO=releases" >> $AFN_TEMP_DIR/target.cfg
    echo "PACKAGING=apu" >> $AFN_TEMP_DIR/target.cfg
    echo "TARGET_DIR=." >> $AFN_TEMP_DIR/target.cfg
    echo "WITH_ARCH=0" >> $AFN_TEMP_DIR/target.cfg
    echo -n "$_AFN_TARGET_ARTIFACT_ID: Fetching latest $_AFN_TARGET_GROUP_ID.$_AFN_TARGET_ARTIFACT_ID..."
    fetchDependency $AFN_TEMP_DIR/target.cfg $HOST_ARCH > /dev/null
    echo "Done"
    rm -rf $AFN_TEMP_DIR

    # Check latest version from local repo
    if [ -d "$_AFN_LOCAL_TARGET_PARENT_DIR" ]; then
      _AFN_TARGET_VERSIONS=$(ls $_AFN_LOCAL_TARGET_PARENT_DIR)
      if [ -z "$_AFN_TARGET_VERSIONS" ]; then
        exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
      fi
    else
      exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
    fi

    _AFN_TARGET_VERSION=$(sortVersions $_AFN_TARGET_VERSIONS | tr -s ' ' '\n' | head -n 1)
    if [ -z "$_AFN_TARGET_VERSION" ]; then
      exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
    fi

    _AFN_APU_FILE=$_AFN_TARGET_ARTIFACT_ID-$_AFN_TARGET_VERSION.apu
    _AFN_TARGET_HOME=$_AFN_TARGET_PARENT/$_AFN_TARGET_VERSION

    if ! [ -f $_AFN_TARGET_HOME/target.cfg ]; then
      mkdir -p $_AFN_TARGET_HOME
      installPackage apu $_AFN_LOCAL_TARGET_PARENT_DIR/$_AFN_TARGET_VERSION/$_AFN_APU_FILE $_AFN_TARGET_HOME
    fi
  else
    _AFN_TARGET_HOME=$_AFN_TARGET_PARENT/$_AFN_TARGET_VERSION

    if ! [ -f $_AFN_TARGET_HOME/target.cfg ]; then
      mkdir -p $AFN_TEMP_DIR
      echo "DEPENDENCY_TYPE=repository" > $AFN_TEMP_DIR/target.cfg
      echo "GROUP_ID=$_AFN_TARGET_GROUP_ID" >> $AFN_TEMP_DIR/target.cfg
      echo "ARTIFACT_ID=$_AFN_TARGET_ARTIFACT_ID" >> $AFN_TEMP_DIR/target.cfg
      echo "VERSION=$_AFN_TARGET_VERSION" >> $AFN_TEMP_DIR/target.cfg
      echo "REPO=releases" >> $AFN_TEMP_DIR/target.cfg
      echo "PACKAGING=apu" >> $AFN_TEMP_DIR/target.cfg
      echo "TARGET_DIR=." >> $AFN_TEMP_DIR/target.cfg
      echo "WITH_ARCH=0" >> $AFN_TEMP_DIR/target.cfg
      mkdir -p $_AFN_TARGET_HOME
      echo -n "$_AFN_TARGET_ARTIFACT_ID: Fetching $_AFN_TARGET_GROUP_ID.$_AFN_TARGET_ARTIFACT_ID..."
      fetchAndInstallDependency $AFN_TEMP_DIR/target.cfg $HOST_ARCH $_AFN_TARGET_HOME > /dev/null || exitWithMsg "Unknown target $1." 1
      echo "OK"
      rm -rf $AFN_TEMP_DIR
    fi
  fi

  # Second step fetch toolchain
  #. $_AFN_TARGET_HOME/target.cfg

  #_AFN_TOOLCHAIN_HOME=
  #if [ -n "$TOOLCHAIN_GROUP_ID" ]; then
  #  _AFN_TOOLCHAIN_HOME=~/.apu/toolchains/$(echo $TOOLCHAIN_GROUP_ID | sed -e "s/\./\//g")/$TOOLCHAIN_ARTIFACT_ID/$TOOLCHAIN_VERSION
  #  if [ ! -d "$_AFN_TOOLCHAIN_HOME" -o ! -f "$_AFN_TOOLCHAIN_HOME/toolchain.cfg" ]; then
  #    mkdir -p $AFN_TEMP_DIR
  #    echo "DEPENDENCY_TYPE=repository" > $AFN_TEMP_DIR/toolchain.cfg
  #    echo "GROUP_ID=$TOOLCHAIN_GROUP_ID" >> $AFN_TEMP_DIR/toolchain.cfg
  #    echo "ARTIFACT_ID=$TOOLCHAIN_ARTIFACT_ID" >> $AFN_TEMP_DIR/toolchain.cfg
  #    echo "VERSION=$TOOLCHAIN_VERSION" >> $AFN_TEMP_DIR/toolchain.cfg
  #    echo "REPO=releases" >> $AFN_TEMP_DIR/toolchain.cfg
  #    echo "PACKAGING=apu" >> $AFN_TEMP_DIR/toolchain.cfg
  #    echo "TARGET_DIR=." >> $AFN_TEMP_DIR/toolchain.cfg
  #    echo "WITH_ARCH=1" >> $AFN_TEMP_DIR/toolchain.cfg
  #    mkdir -p $_AFN_TOOLCHAIN_HOME
  #    echo -n "$TOOLCHAIN_ARTIFACT_ID: Fetching $TOOLCHAIN_GROUP_ID.$TOOLCHAIN_ARTIFACT_ID..."
  #    fetchAndInstallDependency $AFN_TEMP_DIR/toolchain.cfg $HOST_ARCH $_AFN_TOOLCHAIN_HOME > /dev/null || exitWithMsg "Unknown toolchain $TOOLCHAIN_GROUP_ID:$TOOLCHAIN_ARTIFACT_ID:$TOOLCHAIN_VERSION." 1
  #    echo "OK"
  #    rm -rf $AFN_TEMP_DIR
  #  fi
  #fi
}

resolveTarget()
{
  _AFN_TARGET=$1
  _AFN_ENV_FILE=$2
  # Parse TARGET
  _AFN_TARGET_GROUP_ID=com.actility.m2m
  _AFN_TARGET_VERSION=

  i=0
  for _AFN_TOKEN in $(echo $_AFN_TARGET | tr -s ":" "\n")
  do
    _AFN_TOKENS[$i]=$_AFN_TOKEN
    i=$(expr $i + 1)
  done

  if [ "x${#_AFN_TOKENS[@]}" = "x1" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-cross-${_AFN_TOKENS[0]}
    _AFN_TARGET=${_AFN_TOKENS[0]}
  elif [ "x${#_AFN_TOKENS[@]}" = "x2" ]; then
    _AFN_TARGET_GROUP_ID=${_AFN_TOKENS[0]}
    _AFN_TARGET_ARTIFACT_ID=target-cross-${_AFN_TOKENS[1]}
    _AFN_TARGET=${_AFN_TOKENS[1]}
  else
    _AFN_TARGET_GROUP_ID=${_AFN_TOKENS[0]}
    _AFN_TARGET_ARTIFACT_ID=target-cross-${_AFN_TOKENS[1]}
    _AFN_TARGET_VERSION=${_AFN_TOKENS[2]}
    _AFN_TARGET=${_AFN_TOKENS[1]}
  fi

  if [ "$_AFN_TARGET" = "$HOST_TARGET" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-$HOST_TARGET
  elif [ "$_AFN_TARGET" = "$HOST_ARCH" ]; then
    _AFN_TARGET_ARTIFACT_ID=target-$HOST_ARCH
  fi

  _AFN_TARGET_PARENT=~/.apu/targets/$(echo $_AFN_TARGET_GROUP_ID | sed -e "s/\./\//g")/$_AFN_TARGET_ARTIFACT_ID

  if [ -z "$_AFN_TARGET_VERSION" ]; then
    # Detect latest version from local APU repository
    _AFN_LOCAL_TARGET_PARENT_DIR=$AFN_MVN_LOCAL_REPO/$(echo $_AFN_TARGET_GROUP_ID | sed -e "s/\./\//g")/$_AFN_TARGET_ARTIFACT_ID

    # Check latest version from local repo
    if [ -d "$_AFN_LOCAL_TARGET_PARENT_DIR" ]; then
      _AFN_TARGET_VERSIONS=$(ls $_AFN_LOCAL_TARGET_PARENT_DIR)
      if [ -z "$_AFN_TARGET_VERSIONS" ]; then
        exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
      fi
    else
      exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
    fi

    _AFN_TARGET_VERSION=$(sortVersions $_AFN_TARGET_VERSIONS | tr -s ' ' '\n' | head -n 1)
    if [ -z "$_AFN_TARGET_VERSION" ]; then
      exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
    fi

    _AFN_TARGET_HOME=$_AFN_TARGET_PARENT/$_AFN_TARGET_VERSION

    if ! [ -f $_AFN_TARGET_HOME/target.cfg ]; then
      exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID)." 1
    fi
  else
    _AFN_TARGET_HOME=$_AFN_TARGET_PARENT/$_AFN_TARGET_VERSION

    if ! [ -f $_AFN_TARGET_HOME/target.cfg ]; then
      exitWithMsg "Unknown target $1 ($_AFN_TARGET_GROUP_ID:$_AFN_TARGET_ARTIFACT_ID:$_AFN_TARGET_VERSION)." 1
    fi
  fi

  # Second step fetch toolchain
  . $_AFN_TARGET_HOME/target.cfg

  _AFN_TOOLCHAIN_HOME=
  if [ -n "$TOOLCHAIN_GROUP_ID" ]; then
    _AFN_TOOLCHAIN_HOME=~/.apu/toolchains/$(echo $TOOLCHAIN_GROUP_ID | sed -e "s/\./\//g")/$TOOLCHAIN_ARTIFACT_ID/$TOOLCHAIN_VERSION
    if [ ! -d "$_AFN_TOOLCHAIN_HOME" -o ! -f "$_AFN_TOOLCHAIN_HOME/toolchain.cfg" ]; then
      exitWithMsg "Unknown toolchain $1 ($TOOLCHAIN_GROUP_ID:$TOOLCHAIN_ARTIFACT_ID:$TOOLCHAIN_VERSION). You must install it and create $_AFN_TOOLCHAIN_HOME/toolchain.cfg file" 1
    fi
  fi

  if [ -n "$TOOLCHAIN_GROUP_ID" ]; then
    cat $_AFN_TOOLCHAIN_HOME/toolchain.cfg >> $_AFN_ENV_FILE
  fi
  echo "export TARGET_HOME=$_AFN_TARGET_HOME" >> $_AFN_ENV_FILE
  echo "export TARGET_GROUP_ID=$_AFN_TARGET_GROUP_ID" >> $_AFN_ENV_FILE
  echo "export TARGET_ARTIFACT_ID=$_AFN_TARGET_ARTIFACT_ID" >> $_AFN_ENV_FILE
  echo "export TARGET_VERSION=$_AFN_TARGET_VERSION" >> $_AFN_ENV_FILE
  cat $_AFN_TARGET_HOME/target.cfg >> $_AFN_ENV_FILE
  echo export TARGET=$_AFN_TARGET >> $_AFN_ENV_FILE
}

buildDependencies()
{
  _AFN_BUILD_DIR=$1
  _AFN_TARGET=$2
  _AFN_CURRENT=$3

  if ! ls $_AFN_CURRENT/$AFN_DEPENDENCIES_SUB_DIR/*.cfg > /dev/null 2>&1
  then
    echo "No dependencies to retrieve"
    return
  fi

  if ! [ -d "$_AFN_BUILD_DIR" ]
  then
    echo "buildDependencies: no BUILD_DIR ($_AFN_BUILD_DIR)" >&2
    return 1
  fi
 
  # first phase: retrieve and install 'repository' dependencies
  echo "Retrieving and installing 'repository' dependencies"
  for _AFN_FILE in $_AFN_CURRENT/$AFN_DEPENDENCIES_SUB_DIR/*.cfg
  do
    . $_AFN_FILE
    if [ "$DEPENDENCY_TYPE" = "repository" ]
    then
      echo -n "$ARTIFACT_ID: Fetching $GROUP_ID.$ARTIFACT_ID..."
      fetchAndInstallDependency $_AFN_FILE $_AFN_TARGET $_AFN_BUILD_DIR || exitWithMsg "Failed" 1
      echo "OK"
    elif [ "$DEPENDENCY_TYPE" = "source" ]
    then
      echo "$ARTIFACT_ID: skipped"
    else
      exitWithMsg "Unknown dependency type ($DEPENDENCY_TYPE); check file $_AFN_FILE" 1
    fi
  done
  
  # second phase: retrieve and compile 'source' dependencies
  echo "Retrieving and compiling 'source' dependencies"
  for _AFN_FILE in $_AFN_CURRENT/$AFN_DEPENDENCIES_SUB_DIR/*.cfg
  do
    . $_AFN_FILE
    if [ "$DEPENDENCY_TYPE" = "repository" ]
    then
      echo "$ARTIFACT_ID: skipped"
    elif [ "$DEPENDENCY_TYPE" = "source" ]
    then
      if ! [ -d "$_AFN_BUILD_DIR/$TARGET_DIR" ]
      then
       # TODO: retrieve sources from SVN
        echo -n "Copying sources for $TARGET_DIR..."
        cp -r $ROOTACT/$TARGET_DIR $_AFN_BUILD_DIR/
        (
          ROOTACT=$_AFN_BUILD_DIR
          cd $ROOTACT/$TARGET_DIR
          ./MAKE
        ) || exitWithMsg "Failed" 1
        echo "OK"
      else
        exitWithMsg "Missing destination target ($_AFN_BUILD_DIR/$TARGET_DIR); check file $_AFN_FILE" 1
      fi
    else
      exitWithMsg "Unknown dependency type ($DEPENDENCY_TYPE); check file $_AFN_FILE" 1
    fi
  done
}

