
include	$(ROOTACT)/rtbase/base/makefile.def

all: parser

OBJS = parser.o codegen.o main.o tokens.o cb.o ../bin/literals.o ../bin/STANDARD_FBS.o ../bin/ACTILITY_FBS.o

CFLAGS = -I../bin -I$(ROOTACT)/rtbase/include -I../astro -I../cal -I../waveGen -g -m32
LDFLAGS += -L$(ROOTACT)/shlib -m32
LIBS=-lrtbase -lrt
CXXFLAGS = $(CFLAGS)

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	flex -o $@ $^

main.o : main.cpp
	$(CC) -c $(CFLAGS) -DMAIN $<

%.o: %.cpp
	g++ -c $(CXXFLAGS) -o $@ $<


parser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
