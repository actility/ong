
EXE=
#VERBOSE=-v
UPPERCASE=-U
COMPXO=$(ROOTACT)/xo/com/MKXO.sh $(VERBOSE) -p -f
LINKXO=$(ROOTACT)/xo/com/xold.x
WATTXOR=sensor.xor


all: $(WATTXOR) modbus.xor 


sensor.xor: sensor.ref misc.ref
	$(COMPXO) sensor

modbus.xor: $(WATTXOR)
	$(LINKXO) -o modbus.xor $(WATTXOR)

clean:
	@(rm *.x *.o *.a *.xor *.i def*.h core 2>/dev/null || exit 0)
