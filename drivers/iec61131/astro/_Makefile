
include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS += -DMAIN -I../unitTest

all : astro.x

astro.x : astro.o ../unitTest/unitTest.o
	$(CC) $(LDFLAGS) -o astro.x astro.o ../unitTest/unitTest.o -lm

run : astro.x
	./astro.x 69 0 2013 5 20
test :
	@./test.sh

clean :
	-rm *.o *.x
