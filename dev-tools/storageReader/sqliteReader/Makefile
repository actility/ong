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
###############################################################################


ROOTACT ?= $(shell cd ../.. && pwd)
OUTPUT_PATH ?= $(ROOTACT)
export OUTPUT_PATH

PROJECTS_DIR = $(ROOTACT)/
REL_PROJ_PATH = storageReader/sqliteReader
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/sqliteReader.x

DYNAMIC_LIBS = \
-L$(ROOTACT)/lib -lrtbase -lxo \
-L$(ROOTACT)/lib -lsqlite3 \
-lrt -lpthread

CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/main.o

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/rtbase/include \
$(ROOTACT)/xo/include \
$(ROOTACT)/include

DEFINES = _LINUX_
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
DEFINES += LINUX_64
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe

