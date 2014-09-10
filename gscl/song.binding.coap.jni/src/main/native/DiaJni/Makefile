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
# id $Id: Makefile 5323 2013-06-25 14:31:38Z JReich $  
# author $Author: JReich $
# version $Revision: 5323 $
# lastrevision $Date: 2013-06-25 16:31:38 +0200 (Tue, 25 Jun 2013) $
# modifiedby $LastChangedBy: JReich $
# lastmodified $LastChangedDate: 2013-06-25 16:31:38 +0200 (Tue, 25 Jun 2013) $

###############################################################################
# Makefile for "CoAP binding" lib sample
# Author: jreich
# Date: 2012-09-04
###############################################################################


ROOT_PATH = $(shell cd ..; pwd)
OUTPUT_PATH ?= $(shell cd $(ROOT_PATH)/../../; pwd)
export OUTPUT_PATH

BIN_TARGET ?= $(OUTPUT_PATH)/../src/main/target/resources/lib/$(ARCH)

PROJECTS_DIR = $(ROOT_PATH)/
REL_PROJ_PATH = DiaJni
CURRENT_PROJ_DIR = $(ROOT_PATH)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/Source
SO_TARGET = $(TARGET_PROJ_DIR)/$(ARCH)/sharedlib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/$(ARCH)/obj/

# Executable name
SONAME = $(SO_TARGET)/libDiaJni.so

LASTACTIONS = \
if [ -n "$(BIN_TARGET)" ]; \
 then mkdir -p $(BIN_TARGET)/ && cp -vf $(SONAME) $(BIN_TARGET)/ ; \
 else echo "warning: no BIN_TARGET provided... generated binary file not copied"; \
 fi

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/diajni.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/include/linux

INCLUDES += \
$(ROOTACT)/rtbase/include/ \
$(ROOTACT)/libdIa/ \
$(ROOTACT)/libcoap/

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB

DYNAMIC_LIBS = \
 -L$(ROOTACT)/shlib/ \
 -ldIa -lrtbase

include $(ROOT_PATH)/Makefile.common.compilation
include $(ROOT_PATH)/Makefile.common.sharedlib


