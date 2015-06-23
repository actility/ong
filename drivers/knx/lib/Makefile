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

PROJECTS_DIR = $(ROOTACT)/knx
REL_PROJ_PATH = knx/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

PRELIMINARY = \

# Executable name
LIBNAME = $(A_TARGET)/libknx.a

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/DB-dpt.o \
$(OBJS_TARGET)/DB-manufacturer.o \
$(OBJS_TARGET)/DB-range.o \
$(OBJS_TARGET)/var-helpers.o \
$(OBJS_TARGET)/fp16.o \
$(OBJS_TARGET)/iso8601-ext.o \
$(OBJS_TARGET)/iconv.o \
$(OBJS_TARGET)/knx2obix-bits.o \
$(OBJS_TARGET)/knx2obix.o \
$(OBJS_TARGET)/obix2knx-bits.o \
$(OBJS_TARGET)/obix2knx.o \
$(OBJS_TARGET)/eib-client.o \
$(OBJS_TARGET)/version.o \
$(OBJS_TARGET)/list_sort.o \
$(OBJS_TARGET)/diatopology.o \
$(OBJS_TARGET)/covconfig.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/include \
$(PROJECTS_DIR)/supervisor \
$(ROOTACT)/include \
$(ROOTACT)/rtbase/include \
$(ROOTACT)/xo/include \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/drvcommon/lib

DEFINES = _LINUX_
PROJ_DEFINES = SHARED_LIB 
ifeq ($(TARGET),centos6-x86_64)
DEFINES+= LINUX_64
endif

EXTRA_CFLAGS=-D__KNX_DRIVER_VERSION__=`cat $(ROOTACT)/knx/Version`
export EXTRA_CFLAGS

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.staticlib

all: $(CURRENT_PROJ_DIR)/libknx.h build_a_target build_o_target $(LIBNAME)
	@ echo -e "\033[0;1m$(LIBNAME) done\033[0m"

$(OBJS_TARGET)/version.o: ../Version

$(CURRENT_PROJ_DIR)/libknx.h: $(CURRENT_PROJ_DIR)/*.c
	@ echo -e "\033[0;1mGenerating $@\033[0m"
	PATH=$(ROOTACT)/bin:$$PATH cproto -c $(INC_OPTIONS) -o $@ $^

