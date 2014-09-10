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

PROJECTS_DIR = $(ROOTACT)/libSongOverHttp
REL_PROJ_PATH = libSongOverHttp/lib
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
SO_TARGET = $(TARGET_PROJ_DIR)/lib/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
SONAME = $(SO_TARGET)/libSongOverHttp.so
MKWHAT = $(ROOTACT)/rtbase/shells/mkwhat.sh

PRELIMINARY = \
 $(MKWHAT) -p libSongOverHttp -vf ../Version > _whatstr.h

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
-L$(ROOTACT)/shlib -lrtbase \
-L$(ROOTACT)/lib/ -lcurl -lmicrohttpd 

CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/header_t.o \
$(OBJS_TARGET)/httpClient_t.o \
$(OBJS_TARGET)/httpd_impl.o \
$(OBJS_TARGET)/response_t.o \
$(OBJS_TARGET)/freeCltTransactionsList_t.o \
$(OBJS_TARGET)/cltTransactionsList_t.o \
$(OBJS_TARGET)/cltTransaction_t.o \
$(OBJS_TARGET)/srvTransactionsList_t.o \
$(OBJS_TARGET)/srvTransaction_t.o \
$(OBJS_TARGET)/song_over_http_api.o

INCLUDES = \
$(SRC_PATH) \
$(PROJECTS_DIR)/include \
$(ROOTACT)/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.sharedlib


