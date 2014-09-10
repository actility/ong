include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-fPIC -I$(ROOTACT)/rtbase/include -DCOAP_RETRANSMIT -DCOAP_MAX_PDU_SIZE=1400 -DMAX_REQUESTS=50
#CFLAGS+=-fPIC -I$(ROOTACT)/rtbase/include
LDFLAGS+=-L$(ROOTACT)/shlib
ifeq ($(SYSTEM),macosx_x86)
	CFLAGS+=-fnested-functions
endif

all : microdIa.o client.x server.x

client.x : client.o microdIa.o
	$(CC) $(LDFLAGS) -o client.x client.o microdIa.o -lrtbase

server.x : server.o microdIa.o
	$(CC) $(LDFLAGS) -o server.x server.o microdIa.o -lrtbase

%.o     : %.c
	$(CC) $(CFLAGS) -c $<

clean   :
	@(rm *.x *.o *.a *.so *.dylib core _whatstr.h 2>/dev/null || exit 0)
