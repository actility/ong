
include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS += -DMAIN -I$(ROOTACT)/rtbase/include -I../bin -I../unitTest -I$(ROOTACT)/include

all : cal.x

cal.x : cal.o ../unitTest/unitTest.o
	$(CC) $(LDFLAGS) -o cal.x cal.o ../bin/literals.o ../unitTest/unitTest.o -lm

run : cal.x
	./cal.x

test :
	@./test.sh

clean :
	-rm *.o cal.x
