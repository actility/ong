

CFLAGS= -fPIC -shared -nostartfiles

all: libextension2.so copy

libextension2.so : extension2.c
	$(CC) $(CCFLAGS) $(CFLAGS) $(INC) -o $@ $<

copy:
	mkdir -p ../lib/preprocessor/
	cp -vf *.so ../lib/preprocessor/

clean:
	rm -f *.so

