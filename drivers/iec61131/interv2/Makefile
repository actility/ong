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
REL_PROJ_PATH = iec61131/interv2
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/:$(ROOTACT)/iec61131/bin
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/parser.x

ifeq ($(UNARY_TEST),on)
LASTACTIONS = \
cp -vf $(CURRENT_PROJ_DIR)/test.sh $(EXE_TARGET) && \
cp -vfr $(CURRENT_PROJ_DIR)/tests $(EXE_TARGET) && \
cp -vf $(CURRENT_PROJ_DIR)/VG $(EXE_TARGET) && \
cd $(EXE_TARGET) && \
LD_LIBRARY_PATH=$(ROOTACT)/shlib:$(ROOTACT)/lib ./test.sh
endif

STATIC_LIBS= \
$(OUTPUT_PATH)/iec61131/unitTest/lib/libunitTest.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/shlib -lrtbase \
-lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/parser.o \
$(OBJS_TARGET)/codegen.o \
$(OBJS_TARGET)/main.o \
$(OBJS_TARGET)/tokens.o \
$(OBJS_TARGET)/cb.o

OBJS += \
$(OBJS_TARGET)/literals.o \
$(OBJS_TARGET)/STANDARD_FBS.o \
$(OBJS_TARGET)/STANDARD_FUNCTIONS.o \
$(OBJS_TARGET)/ACTILITY_FBS.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/iec61131/unitTest \
$(ROOTACT)/iec61131/bin \
$(ROOTACT)/iec61131/astro \
$(ROOTACT)/iec61131/cal \
$(ROOTACT)/iec61131/waveGen \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe


$(CURRENT_PROJ_DIR)/parser.c: $(CURRENT_PROJ_DIR)/parser.y
	bison -d -o $@ $^

$(CURRENT_PROJ_DIR)/parser.h: $(CURRENT_PROJ_DIR)/parser.c

$(CURRENT_PROJ_DIR)/tokens.c: $(CURRENT_PROJ_DIR)/tokens.l $(CURRENT_PROJ_DIR)/parser.h
	flex -o $@ $^

$(OBJS_TARGET)/main.o: $(CURRENT_PROJ_DIR)/main.c
	$(CC) $(CFLAGS) -DMAIN $(DEF_OPTIONS) $(INC_OPTIONS) -c -o $@ $<

clean:
	@ rm -vrf $(OBJS_TARGET)
	@ rm -vrf $(EXE_TARGET)
	@ rm -vf $(CURRENT_PROJ_DIR)/parser.c $(CURRENT_PROJ_DIR)/parser.h $(CURRENT_PROJ_DIR)/tokens.c


