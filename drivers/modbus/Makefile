# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$


ROOTACT ?= $(shell cd .. && pwd)
SUBDIRS = lib supervisor xoref tests 

include $(ROOTACT)/Makefile.common.recurse

doc: 
	mkdir -p doc
	sed -i -e "s/PROJECT_NUMBER.*=.*/PROJECT_NUMBER = `cat Version`/" modbus.Doxyfile
	doxygen modbus.Doxyfile

