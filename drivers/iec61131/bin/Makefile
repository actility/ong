
include	$(ROOTACT)/rtbase/base/makefile.def

# EXIP
#EXIP=$(ROOTACT)/exip
#LIBS += $(EXIP)/bin/lib/libexip.a
#CFLAGS += -DWITH_EXI -I$(EXIP)/src/grammar/include -I$(EXIP)/include -I$(EXIP)/build/gcc/pc -I$(EXIP)/src/stringTables/include
#OBJS += exi2xml.o

LIBS += -ldIa -lrtbase -lmxml $(MALLOC) -lm -lrt

OBJS += plc.o main.o STANDARD_FUNCTIONS.o STANDARD_FBS.o literals.o admin_cli.o cli.o ACTILITY_FBS.o m2mAccess.o mxml-reduce.o iec_config.o board.o vasprintf.o watchdog.o cpu.o tools.o operating.o validation.o compileST.o xpath.o bucket.o

# ST interpreter
INTER=../interv2
OBJS += $(INTER)/parser.o $(INTER)/codegen.o $(INTER)/tokens.o inter_cb.o do_inter.o

# day/night functions

CFLAGS+=-I$(ROOTACT)/rtbase/include -DMXML -I$(ROOTACT)/libcoap -I$(ROOTACT)/libdIa -I../astro -I../cal -I../waveGen -I$(INTER) -I. -I$(ROOTACT)/include -pthread
LDFLAGS+=-L$(ROOTACT)/shlib -pthread

#CFLAGS += -D__NO_TRACE__

ifeq ($(SYSTEM),macosx_x86)
        CFLAGS+=-fnested-functions
endif

all : plc.x


plc.x : $(OBJS)
	$(CC) $(LDFLAGS) -o plc.x $(OBJS) $(LIBS)

plc.o : plc.c iec_std_lib.h

STANDARD_FUNCTIONS.o : STANDARD_FUNCTIONS.h

main.o : main.c _version.h

literals.o : literals.c iec_std_lib.h

_version.h : ../Version
	$(ROOTACT)/rtbase/shells/mkwhat.sh -p iec61131 -vf ../Version > _version.h

test :
	@./test.sh

.c.o :
	$(CC) $(CFLAGS) -c $*.c

clean :
	-rm *.o *.x _version.h
