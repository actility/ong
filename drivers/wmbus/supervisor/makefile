################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/wmbus/base/makefile.def

################################################################################
LIB=\
$(ROOTACT)/wmbus/lib/libwmbus.a

INC=\
-I../include \
-I$(ROOTACT)/wmbus/lib \
-I$(ROOTACT)/xo/include \
-I$(ROOTACT)/libdIa \
-I$(ROOTACT)/libcoap \
$(PRJINC)

SHLIB= -L $(ROOTACT)/shlib -lrtbase -lxo -ldIa -ldrvcommon

EXTRALD = -ldl -lm

################################################################################
EXE=wmbus

SRC= \
admcli.c \
sensor.c \
config.c \
var.c \
main.c \
covconfig.c
	
OBJ=$(SRC:.c=.o)

CCFLAGS += -O0 -g

#
#	size awspv.x => 
#	   text	   data	    bss	    dec	    hex		filename
#	   1461091 17664    94212   1572967 180067	awspv.x
#
#	./configure --with-minimum --with-reader --with-writer --with-regexps 
#	--with-threads --with-thread-alloc CFLAGS=-m32
#
#	xmlreader.c comment node = (xmlNodePtr) xmlCopyDtd((xmlDtdPtr) node);
#
#
#XML2LD=
#LIBXML2=$(ROOTACT)/libxml2-2.7.8/.libs/libxml2.a
#STATIC=-static -static-libgcc -pthread
#

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ)  $(LIB) 
	$(CC) $(STATIC) $(LDFLAGS) $(XML2LD) -o $@ $^ $(EXTRALD) $(LIBXML2) $(SHLIB)

%.o: ../include/_whatstr.h struct.h define.h state.h diadef.h
%.o: %.c
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean:
	@(rm _cproto.h *.x *.o *.a core *.log 2>/dev/null || exit 0)

cproto.h: $(SRC)
	cproto -c $(INC) $(DEFINC) -o $@ $^

info:
	@(echo $(EXE).x)
