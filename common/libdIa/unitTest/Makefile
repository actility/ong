include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-fPIC -I$(ROOTACT)/libcoap -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libdIa
CFLAGS += -I$(ROOTACT)/iec61131/unitTest
LDFLAGS+=-L$(ROOTACT)/shlib
LIBCOAP=$(ROOTACT)/libcoap/libcoap.a
OBJS=unitTest.o $(ROOTACT)/iec61131/unitTest/unitTest.o

all : unitTest.x

unitTest.x : $(OBJS)
	$(CC) $(LDFLAGS) -o unitTest.x $(OBJS) -ldIa -lrtbase

test :
	@./unitTest.x 2>/dev/null

%.o     : %.c
	$(CC) $(CFLAGS) -c $<

clean   :
	@(rm *.x *.o core 2>/dev/null || exit 0)
	@rm -r root 2>/dev/null
