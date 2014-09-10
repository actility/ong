
ROOTACT ?= $(cd ../..; pwd) 

################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

################################################################################
LIB = \

SHLIB = \
-L$(ROOTACT)/shlib -lrtbase -lxo -lpthread

INC = \
-I. \
-I$(ROOTACT)/rtbase/include \
-I$(ROOTACT)/xo/include

EXTRALD =
ifeq ($(SYSTEM),cov1)
EXTRALD += -lpthread
endif


################################################################################
EXE = sqliteReader

SRC = \
sqlite3.c \
main.c

OBJ = $(SRC:.c=.o)


################################################################################
all : $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@$(NORM)")
	$(CC) $(SHLIB) $(STATIC) $(LDFLAGS) $(XML2LD) $(EXTRALD) -o $@ $^ $(LIBXML2)
	cp $@ $(ROOTACT)/bin/

%.o	: %.c $(foreach file,$(shell ls -1 *.h 2> /dev/null),$(file))
	@(echo -e "$(BLINK)>>> compile $<$(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean :
	@(echo -e "$(BLINK)>>> clean$(NORM)")
	@(rm -f *.x *.o *.a core *.log _whatstr.h 2>/dev/null || exit 0)

info :
	@(echo -e "$(BLINK)>>> info$(NORM)")
	@(echo $(EXE).x)


