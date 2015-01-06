# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$


ROOTACT ?= $(shell cd .. && pwd)
SUBDIRS = include lib supervisor xoref tests 

include $(ROOTACT)/Makefile.common.recurse

doc: 
	sed -i -e "s/PROJECT_NUMBER.*=.*/PROJECT_NUMBER = 0.0.1/" doc/knx.Doxyfile
	doxygen doc/knx.Doxyfile

