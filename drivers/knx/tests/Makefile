include $(ROOTACT)/knx/base/makefile.def

SRC=test-fp16.c \
    test-db.c \
    test-var-helpers.c \
    test-xo-helpers.c \
    test-knx2obix-bits.c \
    test-knx2obix.c \
    test-obix2knx-bits.c \
    test-obix2knx.c \
    test-covconfig.c
   
EXTRA_LIB=
ifeq ($(SYSTEM),lpv3)
EXTRA_LIB+=-liconv
endif 

SHARED_LIB=-lpthread -lrt -leibclient -lrtbase -lxo -ldIa
DRVCOMMON=$(ROOTACT)/drvcommon/lib/libdrvcommon.a

EXE=$(patsubst %.c, %.x, ${SRC})

all: $(EXE)

%.x: %.c $(KNXLIB)
	$(CC) -g -O0 $(INC) $(LDPATH) -o $@ $^ $(DRVCOMMON) $(KNXLIB) -lm $(SHARED_LIB) $(XML2LD) $(LIBXML2) $(EXTRA_LIB)
	chmod +x $@

clean:
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)



