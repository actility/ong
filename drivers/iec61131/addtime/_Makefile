
include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS += -DMAIN -I$(ROOTACT)/rtbase/include

all : addtime.x gettime.x

addtime.x : addtime.o
	$(CC) $(LDFLAGS) -o addtime.x addtime.o

gettime.x : gettime.o
	$(CC) $(LDFLAGS) -o gettime.x gettime.o -lrt

test :
	@./test.sh

clean :
	-rm *.o *.x
