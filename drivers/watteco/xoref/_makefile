################################################################################
include $(ROOTACT)/rtbase/base/makefile.def


## do not clear *.xor because of cross compilation

################################################################################
################################################################################
################################################################################
EXE=
#VERBOSE=-v
UPPERCASE=-U
COMPXO=$(ROOTACT)/xo/com/MKXO.sh $(VERBOSE) -p -f
LINKXO=$(ROOTACT)/xo/com/xold.x

################################################################################

WATTXOR=sensor.xor


all	: $(WATTXOR) watteco.xor 


sensor.xor	: sensor.ref misc.ref
	$(COMPXO) sensor

watteco.xor	: $(WATTXOR)
	$(LINKXO) -o watteco.xor $(WATTXOR)

#clean	:
#	@(rm *.x *.o *.a *.xor *.i def*.h core 2>/dev/null || exit 0)

clean	:
	@(rm *.x *.o *.a *.i def*.h core 2>/dev/null || exit 0)
