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
REL_PROJ_PATH = watteco/supervisor
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/awspv.x

STATIC_LIBS= \
$(OUTPUT_PATH)/watteco/tictools/lib/libtictools.a \
$(OUTPUT_PATH)/watteco/lib/lib/libwatteco.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/shlib -ldrvcommon -lxo -ldIa -lrtbase -lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/http.o \
$(OBJS_TARGET)/admcli.o \
$(OBJS_TARGET)/zcludp.o \
$(OBJS_TARGET)/sensor.o \
$(OBJS_TARGET)/tictools.o \
$(OBJS_TARGET)/config.o \
$(OBJS_TARGET)/covconfig.o \
$(OBJS_TARGET)/var.o \
$(OBJS_TARGET)/diaucb.o \
$(OBJS_TARGET)/main.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/watteco/include \
$(ROOTACT)/drvcommon/lib \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ 
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe


