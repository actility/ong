include $(ROOTACT)/rtbase/base/makefile.def

COAP=$(ROOTACT)/libcoap_29Oct2013
COAP=$(ROOTACT)/libcoap

LIBCOAP_VERSION=$(shell grep VERSION $(COAP)/Makefile | cut --delimiter='=' -f2 | cut --delimiter="." -f1)
ifeq ($(LIBCOAP_VERSION),)
	LIBCOAP_VERSION=2
endif

CFLAGS+=-fPIC -I$(COAP) -I$(ROOTACT)/rtbase/include -DLIBCOAP_VERSION=$(LIBCOAP_VERSION) -DACTILITY_CHANGES
ifeq ($(SYSTEM),macosx_x86)
	CFLAGS+=-fnested-functions
endif
LDFLAGS+=-L$(ROOTACT)/shlib
LIBOBJS=dIa.o dnscache.o options.o version.o hashtbl.o
LIBCOAP=$(COAP)/libcoap.a

all : libdIa.so libdIa.a

libdIa.so : $(LIBOBJS) $(LIBCOAP)
	$(CC) -shared $(LDFLAGS) -o libdIa.so $(LIBOBJS) $(LIBCOAP) -lrtbase
	cp libdIa.so $(ROOTACT)/shlib

libdIa.a : $(LIBOBJS)
	$(AR) r libdIa.a $(LIBOBJS)

version.o       : version.c _whatstr.h
	$(CC) $(CFLAGS) -c $<

_whatstr.h : Version
	$(MKWHAT) -p dia -vf Version > _whatstr.h


%.o     : %.c
	$(CC) $(CFLAGS) -c $<

clean   :
	@(rm *.x *.o *.a *.so *.dylib core _whatstr.h 2>/dev/null || exit 0)
