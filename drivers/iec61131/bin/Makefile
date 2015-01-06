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
REL_PROJ_PATH = iec61131/bin
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

# Executable name
EXENAME = $(EXE_TARGET)/plc.x

ifeq ($(UNARY_TEST),on)
LASTACTIONS = \
cp -vf $(CURRENT_PROJ_DIR)/test.sh $(EXE_TARGET) && \
cp -vf $(CURRENT_PROJ_DIR)/test.py $(EXE_TARGET) && \
cd $(EXE_TARGET) && \
LD_LIBRARY_PATH=$(ROOTACT)/lib ./test.sh
endif


STATIC_LIBS= \
$(OUTPUT_PATH)/iec61131/libinterv2/lib/libinterv2.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/lib -lmxml \
-L$(ROOTACT)/lib -ldIa -lrtbase -lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/plc.o \
$(OBJS_TARGET)/main.o \
$(OBJS_TARGET)/STANDARD_FUNCTIONS.o \
$(OBJS_TARGET)/STANDARD_FBS.o \
$(OBJS_TARGET)/literals.o \
$(OBJS_TARGET)/admin_cli.o \
$(OBJS_TARGET)/cli.o \
$(OBJS_TARGET)/ACTILITY_FBS.o \
$(OBJS_TARGET)/m2mAccess.o \
$(OBJS_TARGET)/mxml-reduce.o \
$(OBJS_TARGET)/iec_config.o \
$(OBJS_TARGET)/board.o \
$(OBJS_TARGET)/vasprintf.o \
$(OBJS_TARGET)/watchdog.o \
$(OBJS_TARGET)/cpu.o \
$(OBJS_TARGET)/tools.o \
$(OBJS_TARGET)/operating.o \
$(OBJS_TARGET)/validation.o \
$(OBJS_TARGET)/compileST.o \
$(OBJS_TARGET)/xpath.o \
$(OBJS_TARGET)/bucket.o \
$(OBJS_TARGET)/inter_cb.o \
$(OBJS_TARGET)/do_inter.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/iec61131/interv2 \
$(ROOTACT)/iec61131/waveGen \
$(ROOTACT)/iec61131/cal \
$(ROOTACT)/iec61131/astro \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ MXML 
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe

$(OBJS_TARGET)/main.o : main.c _version.h

$(OBJS_TARGET)/literals.o : literals.c iec_std_lib.h

_version.h : ../Version
	$(MKWHAT) -p iec61131 -vf ../Version > _version.h

