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

###############################################################################
# Makefile for "JxoJni" lib sample
# Author: jreich
# Date: 2011-07-27
###############################################################################


ROOT_PATH = $(shell cd ..; pwd)
OUTPUT_PATH ?= $(shell cd $(ROOT_PATH)/../../; pwd)
export OUTPUT_PATH

BIN_TARGET ?= $(OUTPUT_PATH)/../src/main/target/resources/lib/$(ARCH)

PROJECTS_DIR = $(ROOT_PATH)/
REL_PROJ_PATH = JxoJni
CURRENT_PROJ_DIR = $(ROOT_PATH)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/Source
SO_TARGET = $(TARGET_PROJ_DIR)/$(ARCH)/sharedlib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/$(ARCH)/obj/

# Executable name
SONAME = $(SO_TARGET)/libJxoJni.so

LASTACTIONS = \
if [ -n "$(BIN_TARGET)" ]; \
 then mkdir -p $(BIN_TARGET)/ && cp -vf $(SONAME) $(BIN_TARGET)/ ; \
 else echo "warning: no BIN_TARGET provided... generated binary file not copied"; \
 fi

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/jni_xo.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/include/linux

INCLUDES += \
$(ROOTACT)/include/libxml2/ \
$(ROOTACT)/rtbase/include/ \
$(ROOTACT)/xo/include/

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

DYNAMIC_LIBS = \
 -L$(ROOTACT)/lib/ -L$(ROOTACT)/shlib/ \
 -lrtbase -lxml2

ifeq ($(TARGET),centos6-x86)
DYNAMIC_LIBS += -lxoexi
CFLAGS += -DEXI
else
DYNAMIC_LIBS += -lxo
endif

include $(ROOT_PATH)/Makefile.common.compilation
include $(ROOT_PATH)/Makefile.common.sharedlib


