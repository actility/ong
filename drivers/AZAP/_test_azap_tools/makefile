################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/AZAP/makefile.def

################################################################################
#LIB=$(PRJLIB)
LIB=\
$(ROOTACT)/rtbase/lib/librtbase.a

STATIC=\
../lib/azap_tools.o

INC=-I../include -I../lib/

################################################################################
EXE=azap_tools_test

SRC=\
azap_tools_test.c

OBJ=$(SRC:.c=.o)

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@ $(NORM)")
	$(CC) $(STATIC) $(LDFLAGS) -o $@ $^ 

%.o	: %.c ../include/azap_types.h ../include/azap_tools.h ../lib/azap_tools.c
	@(echo -e "$(BLINK)>>> compilation $< $(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)


