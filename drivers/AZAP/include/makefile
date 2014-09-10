#################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/AZAP/makefile.def

#################################################################################
LIB=$(PRJLIB)


#################################################################################
all	: _whatstr.h

_whatstr.h : ../Version
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	$(MKWHAT) -p azap -vf ../Version > _whatstr.h


clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core _whatstr.h 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)
