################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
INC=-I../include $(DEFINC)

################################################################################
LIB=libwatteco
SRC=\
	infoproto.c\
	httpparse.c\
	panaddr.c\
	sensor.c\
	ctools.c\
	decode.c\
	encode.c\
	version.c
OBJ=$(SRC:.c=.o)
################################################################################

all	: $(LIB).a

$(LIB).a : $(OBJ)
	$(AR) $(ARFLAGS) $(LIB).a $(OBJ)

version.o	: version.c ../include/_whatstr.h
	$(CC) $(CCFLAGS) $(INC) -c $<

%.o	: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
