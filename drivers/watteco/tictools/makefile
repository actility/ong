################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def
################################################################################
SRC=\
	tic-formater.c\
	tic-parser.c\
	tic-tools.c
OBJ=$(SRC:.c=.o)
################################################################################


all	: $(OBJ)


%.o	: %.c
	$(CC) $(CCFLAGS) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)

