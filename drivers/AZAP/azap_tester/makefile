################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/AZAP/makefile.def

################################################################################
#LIB=$(PRJLIB)
LIB=\
../lib/libazap.a

SHLIB = \
-L$(ROOTACT)/shlib -lrtbase

INC=\
-I../include \
-I../lib/ \
-I$(ROOTACT)/rtbase/include

################################################################################
EXE=azap_tester

EXTRALD = -lm
ifeq ($(SYSTEM),cov1)
EXTRALD += -lpthread 
endif

SRC=\
admin_cli.c \
tester_req_context.c \
listener.c \
main.c

OBJ=$(SRC:.c=.o)

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@ $(NORM)")
	$(CC) $(STATIC) $(LDFLAGS) -o $@ $^ $(SHLIB) $(EXTRALD) 

%.o	: %.c
	@(echo -e "$(BLINK)>>> compilation $< $(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)


