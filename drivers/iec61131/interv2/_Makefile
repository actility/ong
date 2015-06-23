
include	$(ROOTACT)/rtbase/base/makefile.def

all: parser.x

OBJS = parser.o codegen.o main.o tokens.o cb.o ../bin/literals.o ../bin/STANDARD_FBS.o ../bin/STANDARD_FUNCTIONS.o ../bin/ACTILITY_FBS.o

CFLAGS += -I../bin -I$(ROOTACT)/rtbase/include -I../astro -I../cal -I../waveGen -I$(ROOTACT)/include
LDFLAGS += -L$(ROOTACT)/shlib
LIBS=-lrtbase -lm -lrt

clean:
	$(RM) -rf parser.c parser.h parser.x tokens.c $(OBJS)

parser.c: parser.y
	bison -d -o $@ $^
	
parser.h: parser.c

tokens.c: tokens.l parser.h
	flex -o $@ $^

main.o : main.c
	$(CC) -c $(CFLAGS) -DMAIN $<

test :
	@./test.sh

%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

parser.x: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
