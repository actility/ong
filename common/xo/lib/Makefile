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

BIN_TARGET ?= $(OUTPUT_PATH)/lib/
#INC_TARGET ?= $(OUTPUT_PATH)/include/

PROJECTS_DIR = $(ROOTACT)/xo
REL_PROJ_PATH = xo/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
SO_TARGET = $(TARGET_PROJ_DIR)/lib/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(A_TARGET)/libxo.a
SONAME = $(SO_TARGET)/libxo.so
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

PRELIMINARY = \
 $(MKWHAT) -p xo -vf ../Version > _whatstr.h

LASTACTIONS = \
 if [ -n "$(BIN_TARGET)" ]; \
 then mkdir -p $(BIN_TARGET)/ && cp -vf $(SONAME) $(BIN_TARGET)/ ;\
 else echo "warning: no BIN_TARGET provided... generated binary file not copied"; \
 fi && \
 if [ -n "$(INC_TARGET)" ]; \
 then mkdir -p $(INC_TARGET)/rtbase && cp -vf $(PROJECTS_DIR)/include/*.h $(INC_TARGET)/rtbase/ ;\
 else echo "warning: no INC_TARGET provided... header files not copied"; \
 fi 

DYNAMIC_LIBS=-L$(ROOTACT)/shlib -lxml2

#CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/apitab.o \
$(OBJS_TARGET)/api.o \
$(OBJS_TARGET)/apichang.o \
$(OBJS_TARGET)/apiinfo.o \
$(OBJS_TARGET)/apidump.o \
$(OBJS_TARGET)/apisave.o \
$(OBJS_TARGET)/apiload.o \
$(OBJS_TARGET)/apiascii.o \
$(OBJS_TARGET)/apicompr.o \
$(OBJS_TARGET)/apiwalk.o \
$(OBJS_TARGET)/apiexter.o \
$(OBJS_TARGET)/apiexten.o \
$(OBJS_TARGET)/apiXXXread.o \
$(OBJS_TARGET)/apixmlns.o \
$(OBJS_TARGET)/apixmlread.o \
$(OBJS_TARGET)/apixmlwrit.o \
$(OBJS_TARGET)/apixmlsave.o \
$(OBJS_TARGET)/apixmlobix.o \
$(OBJS_TARGET)/apijsowrit.o \
$(OBJS_TARGET)/apidir.o \
$(OBJS_TARGET)/apishareddico.o \
$(OBJS_TARGET)/apilz.o \
$(OBJS_TARGET)/apisection.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/include \
$(ROOTACT)/rtbase/include \
$(ROOTACT)/include/libxml2

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.sharedlib
include $(ROOTACT)/Makefile.common.staticlib


