
include	$(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-I$(ROOTACT)/rtbase/include -DMXML -DMAIN -I../unitTest -I$(ROOTACT)/include -pthread
LDFLAGS+=-L$(ROOTACT)/shlib -pthread

ifeq ($(SYSTEM),macosx_x86)
        CFLAGS+=-fnested-functions
endif

all : xpathExpr.x

xpathExpr.x : xpathExpr.o ../unitTest/unitTest.o
	$(CC) $(LDFLAGS) -o xpathExpr.x xpathExpr.o ../unitTest/unitTest.o $(MALLOC) -lrtbase -lmxml

test :
	@./test.sh

xtest :
	./xpathExpr.x sample1.xml /foo/bar
	./xpathExpr.x sample2.xml /foo/bar[@attr]
	./xpathExpr.x sample2.xml /foo/bar[@attr='Hello']
	./xpathExpr.x sample2.xml /foo/bar[@attr='World']

.c.o :
	$(CC) $(CFLAGS) -c $*.c

clean :
	-rm *.o *.x
