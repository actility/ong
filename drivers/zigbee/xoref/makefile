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

ZIGBXOR=node.xor


all	: $(ZIGBXOR) zigbee.xor 


node.xor	: node.ref misc.ref
	$(COMPXO) node

zigbee.xor	: $(ZIGBXOR)
	$(LINKXO) -o zigbee.xor $(ZIGBXOR)

clean	:
	@(rm -f *.x *.o *.a *.xor *.i def*.h core 2>/dev/null || exit 0)
