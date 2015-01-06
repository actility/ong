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


ROOTACT ?= $(shell cd ../.. && pwd)
OUTPUT_PATH ?= $(ROOTACT)
export OUTPUT_PATH

BIN_TARGET ?= $(OUTPUT_PATH)/bin/

PROJECTS_DIR = $(ROOTACT)/
REL_PROJ_PATH = iec61131/mxml
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = all_exe

ifeq ($(UNARY_TEST),on)
LASTACTIONS = \
cp -vf $(CURRENT_PROJ_DIR)/test.sh $(EXE_TARGET) && \
cp -vf $(CURRENT_PROJ_DIR)/VG $(EXE_TARGET) && \
cp -vf $(CURRENT_PROJ_DIR)/FILTER $(EXE_TARGET) && \
cp -vf $(CURRENT_PROJ_DIR)/*.xml $(EXE_TARGET) && \
cp -vf $(CURRENT_PROJ_DIR)/*.obx $(EXE_TARGET) && \
cd $(EXE_TARGET) && \
LD_LIBRARY_PATH=$(ROOTACT)/shlib:$(ROOTACT)/lib ./test.sh
endif

STATIC_LIBS=
DYNAMIC_LIBS= \
-L$(ROOTACT)/lib -lmxml \
$(MALLOC) \
-L$(ROOTACT)/shlib -lrtbase \
-lrt -lpthread

EXE_BASENAME=xml lookxml scanObix filterxml base64decode

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/xml.o \
$(OBJS_TARGET)/lookxml.o \
$(OBJS_TARGET)/scanObix.o \
$(OBJS_TARGET)/filterxml.o \
$(OBJS_TARGET)/base64decode.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ MXML
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe

$(foreach exefile,$(EXE_BASENAME),$(exefile)):
	$(CC) -o $(EXE_TARGET)/$@.x $(CFLAGS) $(OBJS_TARGET)/$@.o $(STATIC_LIBS) $(DYNAMIC_LIBS)

all_exe: build_dependencies $(OBJS) $(EXE_BASENAME)
	@ echo done # required to override rule $(EXENAME) in Makefile.common.binexe

.PHONY: all_exe

tests :
	LD_LIBRARY_PATH=$(ROOTACT)/shlib:$(ROOTACT)/lib $(EXE_TARGET)/filterxml.x -f $(CURRENT_PROJ_DIR)/plc.xml -w | xmllint --format -

