include $(ROOTACT)/modbus/makefile.def

EXE=modbus

SRC=\
	admcli.c \
	sensor.c \
	sensor-cache.c \
	sensor-diaserver.c \
	sensor-diaclient.c \
	sensor-events.c \
	sensor-reader.c \
	var.c \
  version.c \
	spv_util.c \
	main.c \

	
OBJ=$(SRC:.c=.o)

CCFLAGS += -O0 -g

all: $(EXE).x

$(EXE).x: _whatstr.h $(OBJ) $(MODBUSLIB)
	$(CC) $(STATIC) $(LDPATH) $(LDFLAGS) -o $@ $^ $(SHLIB) $(XML2LD) $(LIBXML2)

_whatstr.h : ../Version
	$(MKWHAT) -p modbus -vf ../Version > _whatstr.h   

%.o: cproto.h
%.o: %.c
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean:
	@(rm _cproto.h *.x *.o *.a core *.log _whatstr.h 2>/dev/null || exit 0)

cproto.h: $(SRC)
	rm -f cproto.h
	touch cproto.h
	cproto -c $(INC) $(DEFINC) -o $@ $^

run:
	./RUN.sh
