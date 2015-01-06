# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$


ROOTACT ?= $(shell cd .. && pwd)
TESTS_DIRS = $(shell cd $(ROOTACT)/wmbus/ && ls -1d _test_*)
SUBDIRS = include lib supervisor xoref $(TESTS_DIRS)

include $(ROOTACT)/Makefile.common.recurse

doc: 
	mkdir -p doc
	sed -i -e "s/PROJECT_NUMBER.*=.*/PROJECT_NUMBER = `cat Version`/" wmbus.Doxyfile
	doxygen wmbus.Doxyfile
