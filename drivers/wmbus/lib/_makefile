################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/wmbus/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
INC=\
-I../include \
-I.. \
-I$(ROOTACT)/xo/include \
-I$(ROOTACT)/wmbus/supervisor \
$(DEFINC) \
$(PRJINC)

################################################################################
LIB=libwmbus
SRC=\
version.c \
preprocessor.c \
processor.c \
processor_parser.c \
manufacturer.c \
dongle.c \
aes.c \
obix_normalizer.c \
tools.c \
extension_library.c \
extension_manager.c
		
OBJ=$(SRC:.c=.o)
################################################################################

export CCFLAGS DEFINC INC

all	: $(LIB).a

$(LIB).a : $(OBJ)
	$(AR) $(ARFLAGS) $(LIB).a $(OBJ)

version.o	: version.c ../include/_whatstr.h
	$(CC) $(CCFLAGS) $(INC) -c $<

%.o	: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
