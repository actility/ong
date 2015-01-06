
include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS += -DMAIN -I$(ROOTACT)/rtbase/include

all : waveGen.x

waveGen.x : waveGen.o
	$(CC) $(LDFLAGS) -o waveGen.x waveGen.o -lm

test :
	@./test.sh

clean :
	-rm *.o waveGen.x
