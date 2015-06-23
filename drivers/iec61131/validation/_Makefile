
include	$(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-I$(ROOTACT)/rtbase/include -I../bin -I$(ROOTACT)/include -pthread
LDFLAGS+=-L$(ROOTACT)/shlib -pthread

OBJS = validation.o ../bin/validation.o ../bin/compileST.o
OBJS += ../bin/literals.o ../bin/STANDARD_FBS.o ../bin/STANDARD_FUNCTIONS.o ../bin/ACTILITY_FBS.o ../bin/mxml-reduce.o

# ST interpreter
INTER=../interv2
OBJS += $(INTER)/parser.o $(INTER)/tokens.o $(INTER)/codegen.o $(INTER)/cb.o


all : validation.x

validation.x : $(OBJS)
	$(CC) $(LDFLAGS) -o validation.x $(OBJS) $(MALLOC) -lrtbase -lmxml -lm -lrt

test :
	@./test.sh

.c.o :
	$(CC) $(CFLAGS) -c $*.c

clean :
	-rm *.o *.x
