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

BIN_TARGET ?= $(OUTPUT_PATH)/lib/
INC_TARGET ?= $(OUTPUT_PATH)/include/

PROJECTS_DIR = $(ROOTACT)/iec61131
REL_PROJ_PATH = iec61131/interv2
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/iec61131/libinterv2

SRC_PATH = $(CURRENT_PROJ_DIR)/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(A_TARGET)/libinterv2.a

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/parser.o \
$(OBJS_TARGET)/codegen.o \
$(OBJS_TARGET)/tokens.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/bin \
$(PROJECTS_DIR)/astro \
$(PROJECTS_DIR)/cal \
$(PROJECTS_DIR)/waveGen \
$(ROOTACT)/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.staticlib

$(CURRENT_PROJ_DIR)/parser.c: $(CURRENT_PROJ_DIR)/parser.y
	bison -d -o $@ $^
  
$(CURRENT_PROJ_DIR)/parser.h: $(CURRENT_PROJ_DIR)/parser.c

$(CURRENT_PROJ_DIR)/tokens.c: $(CURRENT_PROJ_DIR)/tokens.l $(CURRENT_PROJ_DIR)/parser.h
	flex -o $@ $^

clean:
	@ rm -vrf $(OBJS_TARGET)
	@ rm -vrf $(LIBNAME)
	@ rm -vf $(CURRENT_PROJ_DIR)/parser.c $(CURRENT_PROJ_DIR)/parser.h $(CURRENT_PROJ_DIR)/tokens.c

