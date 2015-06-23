
include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS += -DMAIN -I$(ROOTACT)/rtbase/include -I../bin

all : unitTest.o

test :
	@./test.sh

clean :
	-rm *.o
