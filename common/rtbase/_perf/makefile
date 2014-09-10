################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

################################################################################
LIB=$(DEFLIB)
INC=-I../include

COMPILOPT=$(CFLAGSNOO) -O0
#COMPILOPT=$(CFLAGS)

################################################################################

################################################################################
all	: perf.x

%.x : %.o $(LIB)
	$(CC) $(LDFLAGS) -o $@ $< $(LIB)

%.o	: %.c ../include/_whatstr.h
	$(CC) $(COMPILOPT) $(INC) $(DEFINC) -c $<

clean	:
	@(rm *.x *.o *.a core *.log 2>/dev/null || exit 0)
