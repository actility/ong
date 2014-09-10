################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/AZAP/makefile.def

################################################################################
#LIB=$(PRJLIB)
LIB=\
$(ROOTACT)/AZAP/lib/libazap.a \
$(ROOTACT)/rtbase/lib/librtbase.a

INC=-I../include -I../lib/


################################################################################
EXE=uart_buf_control_test

SRC=\
uart_buf_control_test.c

OBJ=$(SRC:.c=.o)

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@ $(NORM)")
	$(CC) $(STATIC) $(LDFLAGS) -o $@ $^ 

%.o	: %.c ../lib/uart_buf_control.h
	@(echo -e "$(BLINK)>>> compilation $< $(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)


