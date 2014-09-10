

CFLAGS= -fPIC -shared -nostartfiles

all: libextension1.so copy

libextension1.so : extension1.c
	$(CC) $(CCFLAGS) $(CFLAGS) $(INC) -o $@ $<

copy:
	mkdir -p ../lib/preprocessor/
	cp -vf *.so ../lib/preprocessor/

clean:
	rm -f *.so

