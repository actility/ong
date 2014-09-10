################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def

################################################################################
LIB=$(PRJLIB)
INC=-I../include -I$(PRJINC)

################################################################################
EXE=

################################################################################
all	: wrzcl.x rdzcl.x xxcode.x tststruct.x xxzcl.x

xxzcl.x : xxzcl.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

wrzcl.x : wrzcl.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

rdzcl.x : rdzcl.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

xxcode.x : xxcode.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

tststruct.x : tststruct.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

%.o	: %.c ../include/_whatstr.h
	$(CC) $(CCFLAGS) $(INC) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
