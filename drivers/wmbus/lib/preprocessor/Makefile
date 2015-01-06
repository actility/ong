
ROOTACT ?= $(shell cd ../../.. && pwd)
MANUF_DIRS = $(shell cd $(ROOTACT)/wmbus/lib/preprocessor/ && ls -1d manufacturer_*)
SUBDIRS = $(MANUF_DIRS)

include $(ROOTACT)/Makefile.common.recurse

