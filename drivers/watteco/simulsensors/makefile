################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def

################################################################################
LIB=$(PRJLIB)
INC=-I../include -I$(PRJINC)

################################################################################
EXE=

################################################################################
all	: binary.x ntemp.x ticcbe.x ticice.x simul.x cpu.x

binary.x : binary.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

ntemp.x : ntemp.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

ticcbe.x : ticcbe.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

ticice.x : ticice.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

simul.x : simul.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

cpu.x : cpu.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

%.o	: %.c ../include/_whatstr.h
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
