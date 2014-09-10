include $(ROOTACT)/modbus/makefile.def

LIB=libmodbus-acy
SRC=\
	var-helpers.c \
	iso8601-ext.c \
	version.c \
	diaproducts.c \
	products.c \
	appitf.c \
	access.c
	
	
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
