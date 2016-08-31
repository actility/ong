#*******************************************************************************
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
#*******************************************************************************

buildProject()
{
  cd JxoJni

  JAVA_SRC_DIR="../../../../../src/main/java/"
  TARGET_CLASSES_DIR="../../../../../target/classes/"
  NATIVE_SRC_DIR="../../../../../src/main/native/JxoJni/Source"
  JNI_CLASS="com.actility.m2m.xo.jni.JniXo"
  JNI_CLASS_FILE="com/actility/m2m/xo/jni/JniXo.java"
  H_FILE="com_actility_m2m_xo_jni_JniXo.h"
  cd Source
  if [ ! -f $NATIVE_SRC_DIR/$H_FILE -o -n "$([ -f $NATIVE_SRC_DIR/$H_FILE ] && find $JAVA_SRC_DIR/$JNI_CLASS_FILE -newer $NATIVE_SRC_DIR/$H_FILE)" ]; then
    echo "javah -classpath $TARGET_CLASSES_DIR $JNI_CLASS";
    javah -classpath $TARGET_CLASSES_DIR $JNI_CLASS;
    touch $H_FILE;
    cp -f $H_FILE $NATIVE_SRC_DIR
  fi
  cd ..

  make ARCH=$1
}