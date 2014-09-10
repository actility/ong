include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/wmbus/base/makefile.def

INC= \
-I.. \
-I$(ROOTACT)/rtbase/include/

CCFLAGS+= -DWMBUS_TEST -g -O0

ifdef ($(SYSTEM),cov2)
CCFLAGS+= -mcpu=arm920t 
LDFLAGS+= -mcpu=arm920t 
endif

all: clean test_parser

dump_conf: dump_conf.o

test_parser: parser.o ../processor_parser.o ../obix_normalizer.o
	$(CC) $(LDFLAGS) -g -o $@ $^ -lm
	
%.o	: %.c
	$(CC) $(CCFLAGS) $(INC) -c $< -o $@

clean:
	rm -f *.o ../*.o
	
.PHONY: clean
