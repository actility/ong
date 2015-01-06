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

PROJECTS_DIR = $(ROOTACT)/libHttpSubsMgmt
REL_PROJ_PATH = libHttpSubsMgmt/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
A_TARGET = $(TARGET_PROJ_DIR)/lib/
SO_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
LIBNAME = $(SO_TARGET)/libHttpSubsMgmt.a
SONAME = $(SO_TARGET)/libHttpSubsMgmt.so
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

PRELIMINARY = \
 $(MKWHAT) -p libHttpSubsMgmt -vf ../Version > _whatstr.h

LASTACTIONS = \
 if [ -n "$(BIN_TARGET)" ]; \
 then mkdir -p $(BIN_TARGET)/ && cp -vf $(SONAME) $(BIN_TARGET)/ ;\
 else echo "warning: no BIN_TARGET provided... generated binary file not copied"; \
 fi && \
 if [ -n "$(INC_TARGET)" ]; \
 then mkdir -p $(INC_TARGET)/ && cp -vf $(PROJECTS_DIR)/include/*.h $(INC_TARGET)/ ;\
 else echo "warning: no INC_TARGET provided... header files not copied"; \
 fi 

DYNAMIC_LIBS=\
-L$(ROOTACT)/lib -lrtbase \
-L$(ROOTACT)/lib -lxo \
-L$(ROOTACT)/lib/ -lSongOverHttp

CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/common.o \
$(OBJS_TARGET)/http_subs_api.o \
$(OBJS_TARGET)/http_subs_soh_impl.o \
$(OBJS_TARGET)/subscriptionsList_t.o \
$(OBJS_TARGET)/subscription_t.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/include \
$(ROOTACT)/include \
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


