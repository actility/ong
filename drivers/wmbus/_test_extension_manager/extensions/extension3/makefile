

CFLAGS= -fPIC -shared -nostartfiles

all: libextension3.so copy

libextension3.so : extension3.c
	$(CC) $(CCFLAGS) $(CFLAGS) $(INC) -o $@ $<

copy:
	mkdir -p ../lib/preprocessor/
	cp -vf *.so ../lib/preprocessor/

clean:
	rm -f *.so

