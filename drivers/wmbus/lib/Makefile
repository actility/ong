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

PROJECTS_DIR = $(ROOTACT)/wmbus
REL_PROJ_PATH = wmbus/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(A_TARGET)/libwmbus.a
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/version.o \
$(OBJS_TARGET)/preprocessor.o \
$(OBJS_TARGET)/processor.o \
$(OBJS_TARGET)/processor_parser.o \
$(OBJS_TARGET)/manufacturer.o \
$(OBJS_TARGET)/dongle.o \
$(OBJS_TARGET)/aes.o \
$(OBJS_TARGET)/obix_normalizer.o \
$(OBJS_TARGET)/tools.o \
$(OBJS_TARGET)/extension_library.o \
$(OBJS_TARGET)/extension_manager.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/wmbus/include \
$(ROOTACT)/wmbus/supervisor \
$(ROOTACT)/include \
$(ROOTACT)/xo/include \
$(ROOTACT)/drvcommon/lib \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

ifeq ($(TARGET),centos6-x86_64)
DEFINES += LINUX_64
endif

SUBDIRS = preprocessor

all: subdirs build_a_target build_o_target $(LIBNAME)

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.staticlib
include $(ROOTACT)/Makefile.common.recurse

$(OBJS_TARGET)/version.o : $(CURRENT_PROJ_DIR)/version.c $(ROOTACT)/wmbus/include/_whatstr.h

