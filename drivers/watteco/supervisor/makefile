################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def

################################################################################
#LIB=$(PRJLIB)

#DRVCOMMON=drvcommon_v0
#DRVCOMMON=drvcommon

DRVCOMMON=$(ROOTACT)/drvcommon/lib

LIB=\
	$(DRVCOMMON)/libdrvcommon.a		\
	$(ROOTACT)/watteco/lib/libwatteco.a	\
	$(ROOTACT)/xo/lib/libxo.a		\
	$(ROOTACT)/libdIa/libdIa.a		\
	$(ROOTACT)/libcoap/libcoap.a		\
	$(ROOTACT)/rtbase/lib/librtbase.a

INC=-I../include -I$(DRVCOMMON) -I$(PRJINC) \
	-I$(ROOTACT)/xo/include -I$(ROOTACT)/libdIa -I$(ROOTACT)/libcoap


TICOBJS=../tictools/tic-formater.o ../tictools/tic-parser.o ../tictools/tic-tools.o

################################################################################
EXE=awspv

SRC=\
	http.c\
	admcli.c\
	zcludp.c\
	sensor.c\
	tictools.c\
	config.c\
	covconfig.c\
	var.c\
	diaucb.c\
	main.c

OBJ=$(SRC:.c=.o)

HDEPS=../include/_whatstr.h\
	$(DRVCOMMON)/cmnstruct.h $(DRVCOMMON)/cmndefine.h $(DRVCOMMON)/diadef.h\
	define.h state.h

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
#LIBXML2=$(ROOTACT)/libxml2-2.7.8/.libs/libxml2.a
#LIBXML2=/usr/lib/libxml2.a
#XML2LD=
#STATIC=-static -static-libgcc -pthread
#

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(TICOBJS) $(LIB)
	$(CC) $(STATIC) $(LDFLAGS) -o $@ $^ $(LIBXML2) $(XML2LD) $(THREADLD) -lm
	-rm tmodel.x
	@(ln $(EXE).x tmodel.x 2>/dev/null)

%.o	: %.c $(HDEPS)
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(rm _cproto.h *.x *.o *.a core *.log 2>/dev/null || exit 0)

#cproto	: 
#	cproto -c $(INC) $(DEFINC) -o _cproto.h *.c

cproto.h	: $(SRC)
	cproto -c $(INC) $(DEFINC) -o $@ $^

info	:
	@(echo $(EXE).x)
