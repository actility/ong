#
# <Copyright>
#
# id $Id: Makefile 1685 2012-01-20 17:18:18Z MLouiset $  
# author $Author: MLouiset $
# version $Revision: 1685 $
# lastrevision $Date: 2012-01-20 18:18:18 +0100 (Fri, 20 Jan 2012) $
# modifiedby $LastChangedBy: MLouiset $
# lastmodified $LastChangedDate: 2012-01-20 18:18:18 +0100 (Fri, 20 Jan 2012) $

###############################################################################
# Author: mlouiset
# Date: 2013-10-03
###############################################################################


ROOT_PATH = $(shell cd .. && pwd)
OUTPUT_PATH ?= $(ROOT_PATH)

SRC_PATH = $(ROOT_PATH)/libiconv/libiconv-1.11/src
SO_TARGET = $(OUTPUT_PATH)/libiconv/libiconv-1.11/$(ARCH)/sharedlib/
OBJS_TARGET = $(OUTPUT_PATH)/libiconv/libiconv-1.11/$(ARCH)/obj/

PRELIMINARY =

# Executable name
SONAME = $(SO_TARGET)/libiconv.so


# List of object file needed for the final program
OBJS = \
$(OBJS_TARGET)/iconv.o 

INCLUDES = \
$(SRC_PATH)/include

DEFINES = _LINUX_ 
PROJ_DEFINES = SHARED_LIB 

STATIC_LIBS = 
DYNAMIC_LIBS =
SUBDIRS_DEP =

include $(ROOT_PATH)/Makefile.common.compilation
include $(ROOT_PATH)/Makefile.common.sharedlib

install:
	mkdir -p $(OUTPUT_PATH)/lib
	install -m 755 $(SONAME) $(OUTPUT_PATH)/lib
	mkdir -p $(OUTPUT_PATH)/include/charmaps
	install -m 644 $(SRC_PATH)/include/*.h $(OUTPUT_PATH)/include
	install -m 644 $(SRC_PATH)/include/charmaps/*.h $(OUTPUT_PATH)/include/charmaps

