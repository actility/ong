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
REL_PROJ_PATH = wmbus/_test_extension_manager
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/test_extension_manager.x

LASTACTIONS_DEP = subdirs

ifeq ($(UNARY_TEST),on)
LASTACTIONS_DEP += unary_tests
endif

STATIC_LIBS= \
$(OUTPUT_PATH)/wmbus/lib/lib/libwmbus.a \
$(ROOTACT)/drvcommon/lib/libdrvcommon.a
DYNAMIC_LIBS = \
-L$(ROOTACT)/lib -lrtbase \
-ldl -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = $(OBJS_TARGET)/test_extension_manager.o 

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include \
$(ROOTACT)/wmbus/lib \
$(ROOTACT)/wmbus/supervisor \
$(ROOTACT)/wmbus/include \
$(ROOTACT)/drvcommon/lib \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
DEFINES += LINUX_64
endif

SUBDIRS = extensions

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe
include $(ROOTACT)/Makefile.common.recurse

unary_tests:
	@echo TESTS
	@for file in $(shell ls -1 $(EXE_TARGET)/*.x); do \
		echo Running $$file ; \
		LD_LIBRARY_PATH=$(ROOTACT)/lib $$file || exit 1 ;\
	done

.PHONY: unary_tests


