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


ROOT_PATH = $(shell cd .. && pwd)
OUTPUT_PATH ?= $(shell cd $(ROOT_PATH)/../.. && pwd)
export OUTPUT_PATH

BIN_TARGET ?= $(OUTPUT_PATH)/$(ARCH)/runEnv

PROJECTS_DIR = $(ROOT_PATH)/
REL_PROJ_PATH = MultiThreadTesterHttpClient
CURRENT_PROJ_DIR = $(ROOT_PATH)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/Source
EXE_TARGET = $(TARGET_PROJ_DIR)/$(ARCH)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/$(ARCH)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/MultiThreadTesterHttpClient.x

LASTACTIONS = \
mkdir -p $(BIN_TARGET) && \
cp -vf $(EXENAME) $(BIN_TARGET) && \
cp -vf $(ROOT_PATH)/lib/* $(BIN_TARGET) 

DYNAMIC_LIBS=-L$(ROOT_PATH)/lib -lcurl -lHttpClient -L$(ROOT_PATH)/lib -lrtbase

CFLAGS += -Wall -Werror -D_NATIVE_TESTER_
export CFLAGS

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/http_clients.o \
$(OBJS_TARGET)/main.o

INCLUDES = \
$(SRC_PATH) \
$(SRC_PATH)/../../HttpClientJni/Source \
$(ROOT_PATH)/include \
$(ROOT_PATH)/include/linux \
$(ROOT_PATH)/rtbase/include

DEFINES = _LINUX_ _NATIVE_TESTER_ 
PROJ_DEFINES = _NATIVE_TESTER_ 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOT_PATH)/Makefile.common.compilation
include $(ROOT_PATH)/Makefile.common.binexe


