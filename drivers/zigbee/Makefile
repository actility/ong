# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$


ROOTACT ?= $(shell cd .. && pwd)
TESTS_DIRS = $(shell cd $(ROOTACT)/zigbee/ && ls -1d _test_*)
SUBDIRS = driver xoref $(TESTS_DIRS)

include $(ROOTACT)/Makefile.common.recurse

doc: 
	mkdir -p doc
	sed -i -e "s/PROJECT_NUMBER.*=.*/PROJECT_NUMBER = `cat Version`/" zigbee.Doxyfile
	doxygen zigbee.Doxyfile
