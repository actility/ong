# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$


ROOTACT ?= $(shell cd .. && pwd)
TESTS_DIRS = $(shell cd $(ROOTACT)/AZAP/ && ls -1d _test_*)
SUBDIRS = include lib azap_tester $(TESTS_DIRS)

include $(ROOTACT)/Makefile.common.recurse

