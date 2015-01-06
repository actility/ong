################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/wmbus/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
LIB=\
$(ROOTACT)/wmbus/lib/libwmbus.a \
$(ROOTACT)/rtbase/lib/librtbase.a

INC=\
-I../include \
-I../lib \
$(DEFINC) \
$(PRJINC)


################################################################################
EXE=test_extension_manager

SRC=\
test_extension_manager.c

OBJ=$(SRC:.c=.o)

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	$(CC) $(STATIC) $(LDFLAGS) -o $@ $^ -ldl 

%.o	: %.c ../lib/extension_library.h ../lib/extension_manager.h
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)


