
ROOTACT ?= $(shell cd ../../../..; pwd)

include $(ROOTACT)/rtbase/base/makefile.def

CFLAGS+=-shared -nostartfiles
INC= \
-I$(ROOTACT)/rtbase/include \
-I$(ROOTACT)/wmbus/include \
-I$(ROOTACT)/drvcommon/lib \
-I$(ROOTACT)/wmbus/lib \
-I$(ROOTACT)/wmbus/lib/preprocessor \
-I$(ROOTACT)/wmbus/supervisor

all: nxp.so

%.so : %.c
	$(CC) $(CCFLAGS) $(CFLAGS) $(INC) -o $@ $< 

clean:
	rm -f *.so

