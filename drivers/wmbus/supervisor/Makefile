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

PROJECTS_DIR = $(ROOTACT)/
REL_PROJ_PATH = wmbus/supervisor
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/wmbus.x

STATIC_LIBS= \
$(OUTPUT_PATH)/wmbus/lib/lib/libwmbus.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/lib -ldrvcommon -lxo -ldIa -lrtbase \
-L$(ROOTACT)/lib -lxml2 \
-ldl -lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/admcli.o \
$(OBJS_TARGET)/sensor.o \
$(OBJS_TARGET)/config.o \
$(OBJS_TARGET)/var.o \
$(OBJS_TARGET)/main.o \
$(OBJS_TARGET)/covconfig.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/wmbus/lib \
$(ROOTACT)/wmbus/include \
$(ROOTACT)/drvcommon/lib \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
DEFINES += LINUX_64
endif

EXTRA_CFLAGS=-D__WMBUS_DRIVER_VERSION__=`cat $(ROOTACT)/wmbus/Version`
export EXTRA_CFLAGS

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe

all: preliminaryAction $(CURRENT_PROJ_DIR)/cproto.h $(CURRENT_PROJ_DIR)/_whatstr.h build_o_target build_exe_target build_dependencies $(EXENAME) lastAction
	@ echo -e "\033[0;1m$(EXENAME) done\033[0m"

$(CURRENT_PROJ_DIR)/_whatstr.h : ../Version
	$(MKWHAT) -p wmbus -vf ../Version > _whatstr.h

$(CURRENT_PROJ_DIR)/cproto.h: $(CURRENT_PROJ_DIR)/*.c
	@ echo -e "\033[0;1mGenerating $@\033[0m"
	PATH=$(ROOTACT)/bin:$$PATH cproto -c $(INC_OPTIONS) -o $@ $^

