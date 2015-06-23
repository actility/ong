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

PROJECTS_DIR = $(ROOTACT)/AZAP
REL_PROJ_PATH = AZAP/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(A_TARGET)/libazap.a
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

DYNAMIC_LIBS=\
-L$(ROOTACT)/lib -lrtbase

CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/postponed_request_elts.o \
$(OBJS_TARGET)/request_context.o \
$(OBJS_TARGET)/request_manager.o \
$(OBJS_TARGET)/azap_attribute.o \
$(OBJS_TARGET)/azap_endpoint.o \
$(OBJS_TARGET)/port_context.o \
$(OBJS_TARGET)/format_znp.o \
$(OBJS_TARGET)/format_znp_sys.o \
$(OBJS_TARGET)/format_znp_zdo.o \
$(OBJS_TARGET)/format_znp_af.o \
$(OBJS_TARGET)/znp_start_sm.o \
$(OBJS_TARGET)/azap_tools.o \
$(OBJS_TARGET)/uart_buf_control.o \
$(OBJS_TARGET)/azap_api.o \
$(OBJS_TARGET)/azap_zcl_attr_val.o \
$(OBJS_TARGET)/azap_api_impl.o \
$(OBJS_TARGET)/version.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/AZAP/include \
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


