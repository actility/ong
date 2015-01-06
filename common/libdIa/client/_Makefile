include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-fPIC -I$(ROOTACT)/libcoap -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libdIa
LDFLAGS+=-L$(ROOTACT)/shlib
LIBCOAP=$(ROOTACT)/libcoap/libcoap.a
OBJS=main.o

all : client.x

client.x : $(OBJS)
	$(CC) $(LDFLAGS) -o client.x $(OBJS) -ldIa -lrtbase

%.o     : %.c
	$(CC) $(CFLAGS) -c $<

clean   :
	@(rm *.x *.o core 2>/dev/null || exit 0)
