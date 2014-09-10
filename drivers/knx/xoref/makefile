################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

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


all	: $(WATTXOR) knx.xor 


sensor.xor	: sensor.ref misc.ref
	$(COMPXO) sensor

knx.xor	: $(WATTXOR)
	$(LINKXO) -o knx.xor $(WATTXOR)

clean	:
	@(rm *.x *.o *.a *.xor *.i def*.h core 2>/dev/null || exit 0)
