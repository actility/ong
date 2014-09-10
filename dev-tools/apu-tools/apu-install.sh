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

CURRENT=`pwd`

if ! type -p curl > /dev/null
then
  echo "oops... curl is not installed on the system..." >&2 
  exit 1 
fi



if [ -z "$ROOTACT" ]
then
  export ROOTACT=$(cd .. && pwd)
  echo "ROOTACT not defined; assuming ROOTACT=$ROOTACT"
  echo "Are you sure you want to install the APU in this root fs?"
  select yn in "Yes" "No"; do
    case $yn in
      Yes ) break;;
      No ) exit;;
    esac
  done
else
  # normalize ROOTACT value
  ROOTACT=$(cd $ROOTACT && pwd)
  if [[ $CURRENT != $ROOTACT* ]]
  then
    echo "*** ROOTACT and current dir doesn't match (ROOTACT=$ROOTACT) (CURRENT=$CURRENT)" >&2 
    exit 1
  fi
fi

# source the configuration file and functions file, 
# either in the same folder then apu-install.sh, either in the current directory.
SELF_NAME=$(basename $0)
SOURCED=false
if [ "$SELF_NAME" == "bash" ] || [ "$SELF_NAME" == "sh" ]
then
  SOURCED=true
  SELF_NAME="apu-install.sh"
fi
SELF_LOCATION=$(which $SELF_NAME 2> /dev/null)


FUNCTIONS_FILE_NAME=apu-functions.sh
CONF_FILE_NAME=apu-tools.conf
CONF_FILE_PATH=.

if [ -n "$SELF_LOCATION" ]
then
  CONF_FILE_PATH=$(dirname $SELF_LOCATION)
fi

if ! [ -f "$CONF_FILE_PATH/$CONF_FILE_NAME" ]
then
  echo "*** Unable to file $CONF_FILE_NAME file in $CONF_FILE_PATH... Aborted." >&2
  exit 1
else
  . $CONF_FILE_PATH/$CONF_FILE_NAME
fi

if ! [ -f "$CONF_FILE_PATH/$FUNCTIONS_FILE_NAME" ]
then
  echo "*** Unable to file $FUNCTIONS_FILE_NAME file in $CONF_FILE_PATH... Aborted." >&2
  exit 1
else
  . $CONF_FILE_PATH/$FUNCTIONS_FILE_NAME
fi


usage() {
  echo "Usage: $SELF_NAME [<options>]"
  echo "  Where options are:"
  echo "   -h|--help                       Print this help message"
  echo
  echo "   -f|--file <apu>                 The APU file to install."
  echo
  echo "   -c|--project-classifier <class> The classifier of the apu to look for."
  echo "                                   Typically, can be the package architecture."
  echo "                                   Can be not provided."
  echo "   -g|--project-group-id <grpId>   The group ID of the apu to look for."
  echo "                                   (default: com.actility)"
  echo "   -n|--project-name <name>        The APU project name to look for."
  echo "   -r|--project-revision <rev>     The APU project package revision to look for."
  echo "                                   If not provided, the script assumes 0 as the"
  echo "                                   APU revision."
  echo "   -v|--project-version <vers>     The APU project version to look for."
  echo
  echo " can be use as follow: "
  echo "  $SELF_NAME -n <name> -v <vers> [-g <grpId>] [-c <class>] [-r <rev>]" 
  echo " or as follow (priority mode): "
  echo "  $SELF_NAME -f <apu file>"
  echo
  echo "WARNING: the script will not install the dependencies."
  echo 
}


###############################################################################

if $SOURCED; then return 0; fi

APU_GROUP_ID="com.actility"
APU_CLASSIFIER=
APU_FILE=
APU_NAME=
APU_REVISION=0
APU_VERSION=

GETOPTTEMP=`getopt -o g:c:f:hn:r:v: --long project-group-id:,project-classifier:,file:,help,project-name:,project-revision:,project-version: -- $@`
if [ $? != 0 ] ; then usage >&2 ; exit 1 ; fi
eval set -- "$GETOPTTEMP"

# Read the arguments
while [ -n "$1" ]
do
  case "$1" in
    "-h"|"--help") usage >&2; exit 0;;
    "-f"|"--file") APU_FILE=$2 ; shift 2 ;;
    "-g"|"--project-group-id") APU_GROUP_ID=$2 ; shift 2 ;;
    "-c"|"--project-classifier") APU_CLASSIFIER=$2 ; shift 2 ;;
    "-n"|"--project-name") APU_NAME=$2 ; shift 2 ;;
    "-r"|"--project-revision") APU_REVISION=$2 ; shift 2 ;;
    "-v"|"--project-version") APU_VERSION=$2 ; shift 2 ;;
    "--") shift; break ;;
    * ) exitWithMsg "Internal error ($*)!" 1;;
  esac
done


if [ -z "$APU_FILE" ]; then

  # try to retrieve the file if not present locally

  # check the mandatory parameters 
  [ -z "$APU_GROUP_ID" ] && usage >&2 && exitWithMsg "*** Group ID parameter not provided. Aborted." 1
  [ -z "$APU_NAME" ] && usage >&2 && exitWithMsg "*** Project name not provided. Aborted." 1
  [ -z "$APU_VERSION" ] && usage >&2 && exitWithMsg "*** Project version not provided. Aborted." 1

  # try to fetch the APU
  _LOCAL_DIR=$LOCAL_REPO/$(echo $APU_GROUP_ID | sed -e "s/\./\//g")/$APU_NAME/$APU_VERSION-$APU_REVISION
  if [ -z "$APU_CLASSIFIER" ]
  then
    _APU_FILE=$APU_NAME-$APU_VERSION-$APU_REVISION.apu
  else
    _APU_FILE=$APU_NAME-$APU_VERSION-$APU_REVISION-$APU_CLASSIFIER.apu
  fi

  if [ ! -f $_LOCAL_DIR/$_APU_FILE ]; then
    echo -n "Fetching file..."
    if [ -z "$APU_CLASSIFIER" ]
    then
      mvnGetDependencyNoArch $APU_GROUP_ID $APU_NAME $APU_VERSION-$APU_REVISION apu || \
      exitWithMsg "KO (APU not found in repository)" 1
    else
      mvnGetDependency $APU_GROUP_ID $APU_NAME $APU_VERSION-$APU_REVISION apu $APU_CLASSIFIER || \
      exitWithMsg "KO (APU not found in repository)" 1
    fi
    echo "OK"
  fi

  APU_FILE=$_LOCAL_DIR/$_APU_FILE
fi

[ ! -f $APU_FILE ] && exitWithMsg "*** File $APU_FILE not found. Aborted." 1
[ ! -f $APU_FILE ] && exitWithMsg "*** File $APU_FILE not found. Aborted." 1

# install the apu 
echo -n "Installing APU..."
installPackage apu $APU_FILE $ROOTACT 
echo "OK"

