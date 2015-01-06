include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-fPIC -I$(ROOTACT)/libcoap -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libdIa
LDFLAGS+=-L$(ROOTACT)/shlib
LIBCOAP=$(ROOTACT)/libcoap/libcoap.a
OBJS=getaddrinfo.o

all : getaddrinfo.x dnscache.x

getaddrinfo.x : $(OBJS)
	$(CC) $(LDFLAGS) -o getaddrinfo.x $(OBJS)

dnscache.x : dnscache.o
	$(CC) $(LDFLAGS) -o dnscache.x dnscache.o -ldIa -lrtbase

%.o     : %.c
	$(CC) $(CFLAGS) -c $<

clean   :
	@(rm *.x *.o core 2>/dev/null || exit 0)
