################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/AZAP/makefile.def

################################################################################
#LIB=$(PRJLIB)
LIB=\
$(ROOTACT)/AZAP/lib/libazap.a \
$(ROOTACT)/rtbase/lib/librtbase.a

INC=-I../include -I../lib/

CFLAGS+=-g -O0

################################################################################
EXE=azap_zcl_attr_val_test

SRC=\
azap_zcl_attr_val_test.c

OBJ=$(SRC:.c=.o)

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@ $(NORM)")
	$(CC) $(STATIC) $(LDFLAGS) -o $@ $^ -lm 

%.o	: %.c
	@(echo -e "$(BLINK)>>> compilation $< $(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)
