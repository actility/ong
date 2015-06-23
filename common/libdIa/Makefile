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


ROOTACT ?= $(shell cd .. && pwd)
OUTPUT_PATH ?= $(ROOTACT)
export OUTPUT_PATH

BIN_TARGET ?= $(OUTPUT_PATH)/lib/
INC_TARGET ?= $(OUTPUT_PATH)/include/

PROJECTS_DIR = $(ROOTACT)/libdIa
REL_PROJ_PATH = libdIa/
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
SO_TARGET = $(TARGET_PROJ_DIR)/lib/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(A_TARGET)/libdIa.a
SONAME = $(SO_TARGET)/libdIa.so
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

LIBCOAP_VERSION=$(shell grep VERSION $(ROOTACT)/libcoap/Makefile | cut -d'=' -f2 | cut -d"." -f1)
ifeq ($(LIBCOAP_VERSION),)
  LIBCOAP_VERSION=2
endif 

PRELIMINARY = \
 $(MKWHAT) -p dia -vf Version > _whatstr.h

STATIC_LIBS=\
$(ROOTACT)/libcoap/libcoap.a

DYNAMIC_LIBS=\
-L$(ROOTACT)/lib -lrtbase

#CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/dIa.o \
$(OBJS_TARGET)/dnscache.o \
$(OBJS_TARGET)/options.o \
$(OBJS_TARGET)/version.o \
$(OBJS_TARGET)/hashtbl.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/include \
$(ROOTACT)/include \
$(ROOTACT)/libcoap/ \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ LIBCOAP_VERSION=$(LIBCOAP_VERSION) 
PROJ_DEFINES = SHARED_LIB 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.sharedlib
include $(ROOTACT)/Makefile.common.staticlib

