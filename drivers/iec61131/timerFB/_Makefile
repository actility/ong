
include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS += -DMAIN -I$(ROOTACT)/rtbase/include -I../bin

LDFLAGS+=-L$(ROOTACT)/shlib

#OBJS=../bin/STANDARD_FBS.o

all : pwm.x

    
pwm.x : pwm.o $(OBJS)
	$(CC) $(LDFLAGS) -o pwm.x $(OBJS) pwm.o  -lrtbase -lmxml -lm -lpthread -lrt 
    

run : pwm.x
	./pwm.x

test :
	@./test.sh

clean :
	-rm *.o pwm.x

