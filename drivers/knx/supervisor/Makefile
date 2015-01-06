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
REL_PROJ_PATH = knx/supervisor
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/knx.x

STATIC_LIBS= \
$(OUTPUT_PATH)/knx/lib/lib/libknx.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/shlib -ldrvcommon -lxo -ldIa -lrtbase \
-L$(ROOTACT)/lib -leibclient -xml2 \
-lm -lrt

ifeq ($(SYSTEM),lpv3)
DYNAMIC_LIBS += -liconv
endif

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/admcli.o \
$(OBJS_TARGET)/sensor.o \
$(OBJS_TARGET)/sensor-diaserver.o \
$(OBJS_TARGET)/sensor-diaclient.o \
$(OBJS_TARGET)/sensor-events.o \
$(OBJS_TARGET)/var.o \
$(OBJS_TARGET)/main.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/knx/lib \
$(ROOTACT)/knx/include \
$(ROOTACT)/drvcommon/lib \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
DEFINES+= LINUX_64
endif

EXTRA_CFLAGS=-D__KNX_DRIVER_VERSION__=`cat $(ROOTACT)/knx/Version`
export EXTRA_CFLAGS

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe

all: preliminaryAction $(CURRENT_PROJ_DIR)/cproto.h build_o_target build_exe_target build_dependencies $(EXENAME) lastAction
	@ echo -e "\033[0;1m$(EXENAME) done\033[0m"

$(CURRENT_PROJ_DIR)/cproto.h: $(CURRENT_PROJ_DIR)/*.c
	@ echo -e "\033[0;1mGenerating $@\033[0m"
	PATH=$(ROOTACT)/bin:$$PATH cproto -c $(INC_OPTIONS) -o $@ $^

