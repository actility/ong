################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/watteco/base/makefile.def

################################################################################
#LIBPCAP=$(ROOTACT)/externals/usr/lib/libpcap.so.1
#LIBNL=$(ROOTACT)/externals/usr/lib/libnl.so.1
INC=-I../include -I../../include
LIBPCAP=$(ROOTACT)/lib/libpcap.a

################################################################################
EXE=

################################################################################
all	: snifwatt.x

#snifwatt.x : snifwatt.o infoproto.o decode.o
#	$(CC) $(LDFLAGS) -o $@ $^ $(LIBPCAP) $(LIBNL)

#snifwatt.x : snifwatt.o infoproto.o decode.o
#	$(CC) $(LDFLAGS) -o $@ $^ $(LIBPCAP)

snifwatt.x : snifwatt.o infoproto.o decode.o
	$(CC) $(LDFLAGS) -L$(ROOTACT)/lib -lpcap -o $@ $^

infoproto.o : ../lib/infoproto.c
	$(CC) $(CCFLAGS) $(INC) -I$(ROOTACT)/rtbase/include -c ../lib/infoproto.c

decode.o : ../lib/decode.c
	$(CC) $(CCFLAGS) $(INC) -I$(ROOTACT)/rtbase/include -c ../lib/decode.c

snifwatt.o : snifwatt.c
	$(CC) $(CCFLAGS) $(INC) -c $<


#%.o	: %.c ../include/_whatstr.h
#	$(CC) $(CCFLAGS) $(INC) -c $<

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
