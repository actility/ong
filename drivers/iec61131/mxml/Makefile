
include	$(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-I$(ROOTACT)/rtbase/include -I$(ROOTACT)/include -DMXML -pthread
LDFLAGS+=-L$(ROOTACT)/shlib
LIBS += -lrtbase -lmxml $(MALLOC) -lm -pthread -lpthread

ifeq ($(SYSTEM),macosx_x86)
        CFLAGS+=-fnested-functions
endif

all : xml.x lookxml.x scanObix.x filterxml.x base64decode.x

xml.x : xml.o
	$(CC) $(LDFLAGS) -o xml.x xml.o $(MALLOC) $(LIBS)

lookxml.x : lookxml.o
	$(CC) $(LDFLAGS) -o lookxml.x lookxml.o $(MALLOC) $(LIBS)

scanObix.x : scanObix.o
	$(CC) $(LDFLAGS) -o scanObix.x scanObix.o $(MALLOC) $(LIBS)

filterxml.x : filterxml.o
	$(CC) $(LDFLAGS) -o filterxml.x filterxml.o $(MALLOC) $(LIBS)

base64decode.x : base64decode.o
	$(CC) $(LDFLAGS) -o base64decode.x base64decode.o $(MALLOC) $(LIBS)

tests :
	./filterxml.x -f plc.xml -w | xmllint --format -

test :
	@./test.sh

.c.o :
	$(CC) $(CFLAGS) -c $*.c

clean :
	-rm *.o *.x
