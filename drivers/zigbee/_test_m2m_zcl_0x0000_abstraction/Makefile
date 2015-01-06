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

PROJECTS_DIR = $(ROOTACT)/
REL_PROJ_PATH = zigbee/_test_m2m_zcl_0x0000_abstraction
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/:$(ROOTACT)/zigbee/driver
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/m2m_zcl_0x0000_abstraction_test.x

ifeq ($(UNARY_TEST),on)
LASTACTIONS = \
LD_LIBRARY_PATH=$(ROOTACT)/shlib $(EXENAME)
endif

STATIC_LIBS = \
$(ROOTACT)/AZAP/lib/libazap.a
DYNAMIC_LIBS = \
-L$(ROOTACT)/shlib -ldrvcommon -lxo -ldIa \
-lrtbase -lm -lrt

ifeq ($(SYSTEM),cov1)
DYNAMIC_LIBS += -lpthread
endif

CFLAGS += -Wall #-Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/m2m_zcl_0x0000_abstraction_test.o

OBJS += \
$(OBJS_TARGET)/azap_listener.o \
$(OBJS_TARGET)/interface_mapping_desc.o \
$(OBJS_TARGET)/version.o \
$(OBJS_TARGET)/zigbee_node.o \
$(OBJS_TARGET)/zigbee_han.o \
$(OBJS_TARGET)/sensor_impl.o \
$(OBJS_TARGET)/var_impl.o \
$(OBJS_TARGET)/dia_impl.o \
$(OBJS_TARGET)/m2m_abstraction.o \
$(OBJS_TARGET)/m2m_zdo_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0000_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0003_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0006_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0400_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0402_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0403_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0404_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0405_abstraction.o \
$(OBJS_TARGET)/m2m_zcl_0x0702_abstraction.o \
$(OBJS_TARGET)/driver_req_context.o \
$(OBJS_TARGET)/main.o 

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/zigbee/driver \
$(ROOTACT)/include \
$(ROOTACT)/AZAP/include \
$(ROOTACT)/drvcommon/lib \
$(ROOTACT)/libdIa \
$(ROOTACT)/libcoap \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_ __UNARY_TEST__
PROJ_DEFINES = 

ifeq ($(TARGET),centos6-x86_64)
DEFINES += LINUX_64
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe


