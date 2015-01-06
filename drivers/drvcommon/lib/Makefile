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

PROJECTS_DIR = $(ROOTACT)/drvcommon
REL_PROJ_PATH = drvcommon/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
SO_TARGET = $(TARGET_PROJ_DIR)/lib/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(A_TARGET)/libdrvcommon.a
SONAME = $(SO_TARGET)/libdrvcommon.so
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

PRELIMINARY = \
 $(MKWHAT) -p drvcommon -vf ../Version > _whatstr.h

DYNAMIC_LIBS=\
-L$(ROOTACT)/shlib -lrtbase -ldIa \

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/diaclient.o \
$(OBJS_TARGET)/diaserver.o \
$(OBJS_TARGET)/diatools.o \
$(OBJS_TARGET)/diakal.o \
$(OBJS_TARGET)/diaipu.o \
$(OBJS_TARGET)/dianet.o \
$(OBJS_TARGET)/diadev.o \
$(OBJS_TARGET)/diaapp.o \
$(OBJS_TARGET)/diadat.o \
$(OBJS_TARGET)/diarep.o \
$(OBJS_TARGET)/diaacc.o \
$(OBJS_TARGET)/admtcp.o \
$(OBJS_TARGET)/ctools.o \
$(OBJS_TARGET)/xmltools.o \
$(OBJS_TARGET)/model.o \
$(OBJS_TARGET)/vartools.o \
$(OBJS_TARGET)/iso8601.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/include \
$(ROOTACT)/include \
$(ROOTACT)/libcoap \
$(ROOTACT)/libdIa \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.sharedlib
include $(ROOTACT)/Makefile.common.staticlib


