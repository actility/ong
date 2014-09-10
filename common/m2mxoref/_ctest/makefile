################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

################################################################################
XML2_LIBS=-lxml2
XML2_INCLUDEDIR=-I/usr/include/libxml2
################################################################################
LIB=$(DEFLIB)
LIBXO=$(ROOTACT)/xo/lib/libxo.a
INC=-I$(ROOTACT)/xo/include $(XML2_INCLUDEDIR)

################################################################################
EXE=
COMPXO=$(ROOTACT)/xo/com/mkxo.x -U -v -f

################################################################################
all	: txo.x

txo.x : txo.o $(LIBXO) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^  $(XML2_LIBS)

txo.o : txo.c
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(rm *.x *.o *.a *.xor core 2>/dev/null || exit 0)
