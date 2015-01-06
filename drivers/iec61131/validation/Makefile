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
REL_PROJ_PATH = iec61131/validation
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

INTER=$(ROOTACT)/iec61131/interv2

SRC_PATH = $(CURRENT_PROJ_DIR)/:$(ROOTACT)/iec61131/bin:$(INTER)
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/validation.x

ifeq ($(UNARY_TEST),on)
LASTACTIONS = \
cp -vf $(CURRENT_PROJ_DIR)/test.sh $(EXE_TARGET) && \
cp -vfr $(CURRENT_PROJ_DIR)/*.xml $(EXE_TARGET) && \
cd $(EXE_TARGET) && \
LD_LIBRARY_PATH=$(ROOTACT)/lib ./test.sh
endif

STATIC_LIBS=
DYNAMIC_LIBS= \
$(MALLOC) \
-L$(ROOTACT)/lib -lmxml \
-L$(ROOTACT)/lib -lrtbase \
-lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/validation2.o

# from iec61131/bin
OBJS += \
$(OBJS_TARGET)/validation.o \
$(OBJS_TARGET)/compileST.o \
$(OBJS_TARGET)/literals.o \
$(OBJS_TARGET)/STANDARD_FBS.o \
$(OBJS_TARGET)/STANDARD_FUNCTIONS.o \
$(OBJS_TARGET)/ACTILITY_FBS.o \
$(OBJS_TARGET)/mxml-reduce.o

# from iec61131/$(INTER)
OBJS += \
$(OBJS_TARGET)/parser.o \
$(OBJS_TARGET)/tokens.o \
$(OBJS_TARGET)/codegen.o \
$(OBJS_TARGET)/cb.o

INCLUDES = \
$(SRC_PATH) \
$(INTER) \
$(ROOTACT)/include \
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

$(INTER)/parser.c: $(INTER)/parser.y
	bison -d -o $@ $^

$(INTER)/parser.h: $(INTER)/parser.c

$(INTER)/tokens.c: $(INTER)/tokens.l $(INTER)/parser.h
	flex -o $@ $^

clean:
	@ rm -vrf $(OBJS_TARGET)
	@ rm -vrf $(EXE_TARGET)
	@ rm -vf $(INTER)/parser.c $(INTER)/parser.h $(INTER)/tokens.c

