#!/bin/sh

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

__VERSION=1.1
CURRENT=`pwd`
SELF_NAME=$(basename $0)
YES_ALL=0
APU_UNPACKING_DIR=".apu_temp"

usage() {
  echo "This tool is designed to help you installing APU files."
  echo "APU, which stands for Actility Package Unit, is the elementary"
  echo "installable component for Actility softwares."
  echo "Version: $__VERSION"
  echo "Usage: $SELF_NAME [<options>] <apu-files>"
  echo "  Where options are:"
  echo "   -h|--help       Print this help message."
  echo "   -y|--yes        Answer yes to all submitted questions."
  echo
  echo "WARNING: this tool doesn't check the dependencies. It doesn't install them neither."
  echo
}

GETOPTTEMP=`getopt -o hy --long help,yes -- $@`
if [ $? != 0 ] ; then usage >&2 ; exit 1 ; fi
eval set -- "$GETOPTTEMP"

# Read the arguments
while [ -n "$1" ]
do
  case "$1" in
    "-h"|"--help") usage >&2; exit 0;;
    "-y"|"--yes") YES_ALL=1 ; shift ;;
    "--") shift; break ;;
    * ) echo "Internal error ($*)!" >&2; exit 1;;
  esac
done

if [ ! -n "$1" ]
then
  echo "*** no apu files provided" >&2
  usage
  exit 1
fi  

if [ -z "$ROOTACT" ]
then
  export ROOTACT=$(pwd)
  echo "ROOTACT not defined; assuming ROOTACT=$ROOTACT"
else
  # normalize ROOTACT value
  ROOTACT=$(cd $ROOTACT && pwd)
  echo "assuming ROOTACT=$ROOTACT"
fi

if [ "x$YES_ALL" == "x0" ]
then
  echo "Are you sure you want to install the APU in this root fs?"
  yn="3"
  while [ "$yn" != "1" -o "$yn" != "2" ]; do
    echo "1) Yes"
    echo "2) No"
    echo -n "#? "
    read yn
    case $yn in
      1 ) break;;
      2 ) echo "Aborted."; exit;;
      * ) ;;
    esac
  done
fi

for apu in $*
do
  ! [ -f $apu ] && echo "$apu: file does not exit; installation skipped..." >&2 && continue
  ! [ -r $apu ] && echo "$apu: cannot access file; installation skipped..." >&2 && continue

  APU_NAME=$(echo $(basename $apu) | \
  sed 's|\([a-zA-Z0-9\_-]*\)\-[0-9]*\.[0-9]*[a-zA-Z0-9\._-]*\.apu|\1|')
  [ -z "$APU_NAME" ] && \
  echo "$apu: cannot detect the name of the apu; installation skipped..." >&2 && continue

  _TEMP_DIR=$ROOTACT/$APU_UNPACKING_DIR/$APU_NAME
  rm -rf $_TEMP_DIR
  mkdir -p $_TEMP_DIR
  echo "--- Unpacking $apu ---"
  tar zxvf $apu -C $_TEMP_DIR 2>&1 || \
  ( echo "$apu: cannot unpack; installation skipped..." >&2 && continue )

  [ ! -f $_TEMP_DIR/control.tar.gz -o ! -f $_TEMP_DIR/data.tar.gz ] && \
  echo "$apu: Unable to find control.tar.gz and data.tar.gz files; installation skipped..." >&2 &&\
  continue

  echo "--- Unpacking control.tar.gz ---"
  tar zxvf $_TEMP_DIR/control.tar.gz -C $_TEMP_DIR/ 2>&1 || \
  ( echo "$apu: cannot unpack control.tar.gz; installation skipped..." >&2 && continue )

  # preinst
  echo "--- Pre-installation ---"
  if [ -f $_TEMP_DIR/preinst ]
  then
    chmod +x $_TEMP_DIR/preinst 2>&1
    $_TEMP_DIR/preinst 2>&1 || \
    ( echo "$apu: preinst failed; installation installation skipped..." >&2 && continue )
  fi

  # install
  echo "--- Installation ---"
  tar zxvf $_TEMP_DIR/data.tar.gz -C $ROOTACT || \
  ( echo "$apu: cannot unpack data.tar.gz; installation skipped..." >&2 && continue )
 
  # postinst
  echo "--- Post-installation ---"
  if [ -f $_TEMP_DIR/postinst ]
  then
    chmod +x $_TEMP_DIR/postinst 2>&1
    $_TEMP_DIR/postinst 2>&1 || \
    ( echo "$apu: postinst failed; installation installation skipped..." >&2 && continue )
  fi 

  echo "$apu: installation completed successfully"
done

# clean-up before leaving
rm -rf $ROOTACT/$APU_UNPACKING_DIR

