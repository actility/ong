#
# <Copyright>
#
# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$

###############################################################################
# Makefile for "Transparent pseudo-terminal (PTY)"
###############################################################################

ROOT_PATH ?= $(shell cd ../.. && pwd)
OUTPUT_PATH ?= $(ROOT_PATH)
REL_PROJ_PATH = trans_pty/TransPtyExe
CURRENT_PROJ_DIR = $(ROOT_PATH)/$(REL_PROJ_PATH)
TARGET_PROJ_DIR = $(OUTPUT_PATH)/$(REL_PROJ_PATH)
BIN_TARGET ?= $(OUTPUT_PATH)/bin/

SRC_PATH = $(CURRENT_PROJ_DIR)/Source
EXE_TARGET = $(TARGET_PROJ_DIR)/bin/

OBJS_TARGET = $(TARGET_PROJ_DIR)/obj/

# Executable name
EXENAME = $(EXE_TARGET)/trans_pty

LASTACTIONS = \
if [ -n "$(BIN_TARGET)" ]; \
 then mkdir -p $(BIN_TARGET)/ && cp -vf $(EXENAME) $(BIN_TARGET)/ ; \
 else echo "warning: no BIN_TARGET provided... generated binary file not copied"; \
fi

# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/main.o

INCLUDES = \
$(SRC_PATH) 

DEFINES = _LINUX_ POSIX 

# config file where ZAP specific options are defined.
ZAP_CONFIG_FILES = 

# List of object file needed for the final program
STATIC_LIBS = 

DYNAMIC_LIBS =

SUBDIRS_DEP = 

include $(ROOT_PATH)/Makefile.common.compilation
include $(ROOT_PATH)/Makefile.common.binexe

