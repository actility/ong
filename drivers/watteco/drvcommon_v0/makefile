################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
#include $(ROOTACT)/watteco/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
INC=-I../include -I$(ROOTACT)/xo/include\
	-I$(ROOTACT)/libdIa -I$(ROOTACT)/libcoap $(DEFINC)

################################################################################
LIB=libdrivercommon
SRC=\
	diaclient.c\
	diaserver.c\
	diatools.c\
	diakal.c\
	diaipu.c\
	dianet.c\
	diadev.c\
	diaapp.c\
	diadat.c\
	diarep.c\
	diaacc.c\
	admtcp.c\
	ctools.c\
	xmltools.c\
	model.c\
	vartools.c\
	iso8601.c
OBJ=$(SRC:.c=.o)
################################################################################

all	: $(LIB).a

$(LIB).a : $(OBJ)
	$(AR) $(ARFLAGS) $(LIB).a $(OBJ)

cmnproto.h	: $(SRC)
	cproto -c $(INC) $(DEFINC) -o $@ $^

%.o	: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
