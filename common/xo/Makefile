

ROOTACT ?= $(shell cd .. && pwd)

ifeq ($(TARGET),centos6-x86)
SUBDIRS = lib libexi com
else
SUBDIRS = lib com
endif

include $(ROOTACT)/Makefile.common.recurse

