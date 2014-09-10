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


all	: $(WATTXOR) wmbus.xor 


sensor.xor	: sensor.ref misc.ref
	$(COMPXO) sensor

wmbus.xor	: $(WATTXOR)
	$(LINKXO) -o wmbus.xor $(WATTXOR)

clean	:
	@(rm *.x *.o *.a *.xor *.i def*.h core 2>/dev/null || exit 0)
