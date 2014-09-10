include $(ROOTACT)/knx/base/makefile.def

LIB=libknx
SRC=\
	DB-dpt.c \
    DB-manufacturer.c \
	DB-range.c \
	var-helpers.c \
	fp16.c \
	iso8601-ext.c \
	iconv.c \
	knx2obix-bits.c \
	knx2obix.c \
	obix2knx-bits.c \
	obix2knx.c \
	eib-client.c \
	version.c \
	list_sort.c \
	diatopology.c \
	covconfig.c \
	
	
OBJ=$(patsubst %.c, %.o, ${SRC})

all: $(LIB).a

$(LIB).a: $(OBJ) $(LIB).h
	$(AR) $(ARFLAGS) $(LIB).a $(OBJ)

$(LIB).h: $(SRC)
	cproto -c $(INC) $(DEFINC) -o $@ $^

version.o: ../Version
%.o: %.c $(LIB).h
	$(CC) $(CCFLAGS) $(INC) -c $<

clean:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
