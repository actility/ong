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
# id $Id: Makefile 1961 2012-03-13 13:24:09Z MLouiset $  
# author $Author: MLouiset $
# version $Revision: 1961 $
# lastrevision $Date: 2012-03-13 14:24:09 +0100 (Tue, 13 Mar 2012) $
# modifiedby $LastChangedBy: MLouiset $
# lastmodified $LastChangedDate: 2012-03-13 14:24:09 +0100 (Tue, 13 Mar 2012) $

###############################################################################
# Makefile for "JniXo" lib sample
# Author: jreich
# Date: 2011-07-27
###############################################################################

ROOT_PATH = $(shell pwd)
OUTPUT_PATH ?= $(ROOT_PATH)
export OUTPUT_PATH

BIN_TARGET ?= $(OUTPUT_PATH)/bin/

PROJECTS_DIR = $(ROOT_PATH)/

SRC_PATH = $(PROJECTS_DIR)/sources
OBJS_TARGET = $(OUTPUT_PATH)/obj/

# Executable name
EXENAME = $(BIN_TARGET)/stdin-logger

PRELIMINARY = mkdir -p $(BIN_TARGET)
LASTACTIONS = 

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/main.o

INCLUDES = \
$(SRC_PATH)

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

include $(ROOT_PATH)/Makefile.common.compilation
include $(ROOT_PATH)/Makefile.common.binexe


