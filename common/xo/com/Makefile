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
REL_PROJ_PATH = xo/com
CURRENT_PROJ_DIR = $(ROOTACT)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)

SRC_PATH = $(CURRENT_PROJ_DIR)/
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/
OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

EXENAME = all_exe

PRELIMINARY = \
 $(YACC) $(YFLAGS) mkxo.y && \
 mv y.tab.c mkxo.c && \
 $(LEX) $(LFLAGS) mkxo.l && \
 mv lex.yy.c mkxo.l.h


# Executable name
OTHER_EXE_BASENAME=xosize xoattr xoload xoinit xodump xoconv\
 xotoasci xordasci xoxml xold xodico xocompr xotst 

ifeq ($(TARGET),centos6-x86)
OTHER_EXE_BASENAME += xordexi
endif

ifeq ($(TARGET),centos6-x86)
STATIC_LIBS=
$(ROOTACT)/xo/libexi/lib/libxoexi.a \
$(ROOTACT)/rtbase/lib/librtbase.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/lib -lexip \
-L$(ROOTACT)/shlib -lxml2 -lrt
else
STATIC_LIBS = \
$(ROOTACT)/xo/lib/lib/libxo.a \
$(ROOTACT)/rtbase/lib/librtbase.a
DYNAMIC_LIBS= \
-L$(ROOTACT)/shlib -lxml2 -lrt
endif

STATIC_LIB=

#CFLAGS += -Wall -Werror

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/xoattr.o \
$(OBJS_TARGET)/mkxo.o \
$(OBJS_TARGET)/usermkxo.o \
$(OBJS_TARGET)/xoload.o \
$(OBJS_TARGET)/xosize.o \
$(OBJS_TARGET)/xoinit.o \
$(OBJS_TARGET)/xodump.o \
$(OBJS_TARGET)/xoconv.o \
$(OBJS_TARGET)/xotoasci.o \
$(OBJS_TARGET)/xordexi.o \
$(OBJS_TARGET)/xordasci.o \
$(OBJS_TARGET)/xoxml.o \
$(OBJS_TARGET)/xold.o \
$(OBJS_TARGET)/xodico.o \
$(OBJS_TARGET)/xocompr.o \
$(OBJS_TARGET)/xotst.o \

INCLUDES = \
$(SRC_PATH) \
$(ROOTACT)/include/libxml2 \
$(ROOTACT)/xo/include \
$(ROOTACT)/rtbase/include

DEFINES = _LINUX_  

ifeq ($(TARGET),centos6-x86_64)
EXTRA_CFLAGS=-DLINUX_64
export EXTRA_CFLAGS
endif
ifeq ($(TARGET),centos6-x86)
DEFINES += XO_WITH_EXIP
endif

include $(ROOTACT)/Makefile.common.compilation
include $(ROOTACT)/Makefile.common.binexe

$(EXE_TARGET)/mkxo.x : $(OBJS_TARGET)/mkxo.o $(OBJS_TARGET)/usermkxo.o
	$(CC) -o $@ $(CFLAGS) $+ $(STATIC_LIBS) $(DYNAMIC_LIBS)

$(foreach exefile,$(OTHER_EXE_BASENAME),$(exefile)):
	$(CC) -o $(EXE_TARGET)/$@.x $(CFLAGS) $(OBJS_TARGET)/$@.o $(STATIC_LIBS) $(DYNAMIC_LIBS)

clean:
	@ rm -vrf $(OBJS_TARGET)
	@ rm -vrf $(EXE_TARGET)
	@ rm -vf y.tab.c mkxo.c lex.yy.c mkxo.l.h

all_exe: build_dependencies $(OBJS) $(EXE_TARGET)/mkxo.x $(OTHER_EXE_BASENAME)
	@ echo done # required to override rule $(EXENAME) in Makefile.common.binexe

.PHONY: all_exe

