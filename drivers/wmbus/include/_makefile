#################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/wmbus/base/makefile.def

#################################################################################
LIB=$(PRJLIB)
INC=-I../include -I$(PRJINC)

#################################################################################
EXE=discover

#################################################################################
all	: _whatstr.h

_whatstr.h : ../Version
	$(MKWHAT) -p wmbus -vf ../Version > _whatstr.h

clean	:
	@(rm *.x *.o *.a core _whatstr.h 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)
