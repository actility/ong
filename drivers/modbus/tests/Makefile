include $(ROOTACT)/modbus/makefile.def

SRC=\
test_lib_access.c \
test_lib_attr_t.c \

#test_test.c
    
SPV_OBJS=\
$(ROOTACT)/modbus/supervisor/var.o \
$(ROOTACT)/modbus/supervisor/version.o \
$(ROOTACT)/modbus/supervisor/sensor-diaserver.o \
$(ROOTACT)/modbus/supervisor/sensor-diaclient.o \
$(ROOTACT)/modbus/supervisor/sensor.o \
$(ROOTACT)/modbus/supervisor/sensor-cache.o \
$(ROOTACT)/modbus/supervisor/sensor-events.o \
$(ROOTACT)/modbus/supervisor/sensor-reader.o \
$(ROOTACT)/modbus/supervisor/admcli.o \
$(ROOTACT)/modbus/supervisor/spv_util.o \
    

EXE=$(patsubst %.c, %.x, ${SRC})

all: $(EXE) #run

%.x: %.c $(SPV_OBJS) $(MODBUSLIB) $(DRVCOMMONLIB)
	$(CC) -g -O0 $(INC) $(LDPATH) $(LDFLAGS) -o $@ $^ -lm $(MODBUSLIB) $(SHLIB) $(DRVCOMMONLIB) $(SHLIB) $(XML2LD) $(LIBXML2)
	chmod +x $@
	@echo $(LD_LIBRARY_PATH)

clean:
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

run: $(EXE)
	for tester in $^ ; do \
		LD_LIBRARY_PATH=$(ROOTACT)/shlib:$(ROOTACT)/lib ./$$tester || exit 1 ;\
	done


