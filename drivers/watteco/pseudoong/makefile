include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def

INC=-I$(ROOTACT)/libcoap -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libdIa
#LDFLAGS+=-L$(ROOTACT)/shlib
#OBJS=main.o
OBJS=main.o $(LIBDIA) $(LIBCOAP) $(LIBBASE)

all : simong.x

#simong.x : $(OBJS)
#	$(CC) $(LDFLAGS) -o simong.x $(OBJS) -ldIa -lrtbase

simong.x : $(OBJS)
	$(CC) $(LDFLAGS) -o simong.x $(OBJS)

%.o     : %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

clean   :
	@(rm *.x *.o core 2>/dev/null || exit 0)
