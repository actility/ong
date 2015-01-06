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
REL_PROJ_PATH = modbus/tests
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/:$(ROOTACT)/modbus/supervisor
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = all_exe

ifeq ($(UNARY_TEST),on)
LASTACTIONS_DEP = unary_tests
endif

STATIC_LIBS= \
$(OUTPUT_PATH)/modbus/lib/lib/libmodbus-acy.a \
$(ROOTACT)/lib/libmodbus.a \
$(ROOTACT)/drvcommon/lib/libdrvcommon.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/lib -lxo -ldIa -lrtbase \
-lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

ifeq ($(SYSTEM),lpv3)
DYNAMIC_LIBS += -liconv
endif

#EXE_BASENAME = $(patsubst $(CURRENT_PROJ_DIR)/%.c, %, ${CURRENT_PROJ_DIR})
EXE_BASENAME = test_lib_access test_lib_attr_t

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = $(foreach item,$(EXE_BASENAME),$(OBJS_TARGET)/$(item).o) 

SPV_OBJS = \
$(OBJS_TARGET)/var.o \
$(OBJS_TARGET)/version.o \
$(OBJS_TARGET)/sensor-diaserver.o \
$(OBJS_TARGET)/sensor-diaclient.o \
$(OBJS_TARGET)/sensor.o \
$(OBJS_TARGET)/sensor-cache.o \
$(OBJS_TARGET)/sensor-events.o \
$(OBJS_TARGET)/sensor-reader.o \
$(OBJS_TARGET)/admcli.o \
$(OBJS_TARGET)/spv_util.o

OBJS += $(SPV_OBJS)

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/modbus/lib \
$(ROOTACT)/modbus/supervisor \
$(ROOTACT)/modbus/include \
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

$(foreach exefile,$(EXE_BASENAME),$(exefile)):
	$(CC) -o $(EXE_TARGET)/$@.x $(CFLAGS) $(OBJS_TARGET)/$@.o $(SPV_OBJS) $(STATIC_LIBS) $(DYNAMIC_LIBS)

all_exe: build_dependencies $(OBJS) $(EXE_BASENAME)
	@ echo done # required to override rule $(EXENAME) in Makefile.common.binexe

unary_tests:
	@echo TESTS
	@for file in $(shell ls -1 $(EXE_TARGET)/*.x); do \
		echo Running $$file ; \
		LD_LIBRARY_PATH=$(ROOTACT)/lib $$file || exit 1 ;\
	done

.PHONY: all_exe unary_tests


