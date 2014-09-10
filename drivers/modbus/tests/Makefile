include $(ROOTACT)/modbus/makefile.def

SRC=\

    

EXE=$(patsubst %.c, %.x, ${SRC})

all: $(EXE)

%.x: %.c $(MODBUSLIB) $(DRVCOMMONLIB)
	$(CC) -g -O0 $(INC) $(LDPATH) -o $@ $^ -lm $(MODBUSLIB) $(DRVCOMMONLIB) $(SHLIB) $(XML2LD) $(LIBXML2)
	chmod +x $@

clean:
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)



