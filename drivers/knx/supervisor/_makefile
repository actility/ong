include $(ROOTACT)/knx/base/makefile.def

EXE=knx

SRC=\
	admcli.c \
	sensor.c \
	sensor-diaserver.c \
	sensor-diaclient.c \
	sensor-events.c \
	var.c \
	main.c

EXTRA_LIB=-lm
ifeq ($(SYSTEM),lpv3)
EXTRA_LIB+=-liconv
endif
	
OBJ=$(SRC:.c=.o)

CCFLAGS += -O0 -g

all: $(EXE).x

$(EXE).x: $(OBJ) $(KNXLIB)
	$(CC) $(STATIC) $(LDPATH) $(LDFLAGS) -o $@ $^ $(SHLIB) $(XML2LD) $(LIBXML2) $(EXTRA_LIB)

%.o: cproto.h
%.o: %.c
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean:
	@(rm _cproto.h *.x *.o *.a core *.log 2>/dev/null || exit 0)

cproto.h: $(SRC)
	rm -f cproto.h
	touch cproto.h
	cproto -c $(INC) $(DEFINC) -o $@ $^

run:
	killall eibd || echo "No eibd process found"
	./RUN.sh
