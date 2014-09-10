include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-fPIC -I$(ROOTACT)/libcoap -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libdIa
LDFLAGS+=-L$(ROOTACT)/shlib
LIBCOAP=$(ROOTACT)/libcoap/libcoap.a
OBJS=split.o

all : split.x

split.x : $(OBJS)
	$(CC) $(LDFLAGS) -o split.x $(OBJS) -ldIa -lrtbase

vg : split.x
	valgrind --tool=memcheck --leak-check=yes ./split.x

%.o     : %.c
	$(CC) $(CFLAGS) -c $<

clean   :
	@(rm *.x *.o core 2>/dev/null || exit 0)
