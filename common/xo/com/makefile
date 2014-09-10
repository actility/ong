

#################################################
#						#
#	Makefile des commandes xo		#
#						#
#################################################

include $(ROOTACT)/rtbase/base/makefile.def

LIBXO=../lib/libxo.a $(DEFLIB)
LIBXOEXI=../libexi/libxoexi.a $(ROOTACT)/exip/bin/lib/libexip.a $(DEFLIB)

#LIB=$(ROOTACT)/shlib/libxo.so /usr/lib32/libxml2.so
#LIB=$(ROOTACT)/shlib/libxo.so $(ROOTACT)/shlib/libexip.so $(ROOTACT)/shlib/librtbase.so

INCLUDE=-I../include -I$(XML2INC) -I$(ROOTACT)/rtbase/include
VAL=

EXE=mkxo.x xosize.x xoattr.x xoload.x xoinit.x xodump.x xoconv.x\
	xotoasci.x xordasci.x xoxml.x xold.x xodico.x\
	xocompr.x xotst.x

ifneq ($(wildcard $(ROOTACT)/exip),)
EXE += xordexi.x
CFLAGS += -DXO_WITH_EXIP
LIB=$(LIBXOEXI)
else
LIB=$(LIBXO)
endif

#all	: mkxo.x xosize.x xoattr.x xoload.x xoinit.x xodump.x xoconv.x\
#	xotoasci.x xordasci.x xoxml.x xold.x xodico.x xordexi.x\
#	xocompr.x xotst.x

all	: $(EXE)

mbr :

#
#	@(cp mkxo.l.h lex_mkxo.l.h 2>/dev/null || exit 0)
#	@(cp mkxo.c yac_mkxo.c 2>/dev/null || exit 0)
#

clean	:
	@(rm *.x *.o mkxo.o mkxo.l.h mkxo.c 2>/dev/null || exit 0)

mkxo.x : mkxo.o usermkxo.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o mkxo.x mkxo.o usermkxo.o $(LIB) $(LIBYACCLD) $(LIBLEX) $(MALLOC) $(SHLIBC) $(XML2LD)

mkxo.c : mkxo.y mkxo.l.h
	$(YACC) $(YFLAGS) mkxo.y
	mv y.tab.c mkxo.c

mkxo.l.h : mkxo.l
	$(LEX) $(LFLAGS) mkxo.l
	mv lex.yy.c mkxo.l.h

xoattr.x:xoattr.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xoattr.x xoattr.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xoload.x:xoload.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xoload.x xoload.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xosize.x:xosize.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xosize.x xosize.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xoinit.x:xoinit.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xoinit.x xoinit.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xodump.x:xodump.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xodump.x xodump.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xoconv.x:xoconv.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xoconv.x xoconv.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xotoasci.x:xotoasci.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xotoasci.x xotoasci.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xordexi.x:xordexi.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xordexi.x xordexi.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xordasci.x:xordasci.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xordasci.x xordasci.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xoxml.x:xoxml.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xoxml.x xoxml.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xold.x:xold.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xold.x xold.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xodico.x:xodico.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xodico.x xodico.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xocompr.x:xocompr.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xocompr.x xocompr.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

xotst.x:xotst.o $(LIB)
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o xotst.x xotst.o $(LIB) $(VAL) $(MALLOC) $(SHLIBC) $(XML2LD)

.c.o :
	$(CC) $(CFLAGS) $(INCLUDE) -c $*.c
