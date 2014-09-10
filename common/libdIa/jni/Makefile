include $(ROOTACT)/rtbase/base/makefile.def

JAVAHOME=/System/Library/Frameworks/JavaVM.framework/Versions/Current
JAVAINC=${JAVAHOME}/Headers
CFLAGS+=-fPIC -I${JAVAINC} -I.. -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libcoap
LDFLAGS+=-L$(ROOTACT)/shlib
LIBS=-ldIa -lrtbase
JAVAFLAGS=-target 1.3 -source 1.3

all : diajni.class libdiajni.so

libdiajni.so : diajni.c diajni.h
	$(CC) $(CFLAGS) -shared diajni.c -o libdiajni.so $(LDFLAGS) $(LIBS)

diajni.h : diajni.java
	javah diajni

diajni.class : diajni.java
	javac $(JAVAFLAGS) diajni.java

install :
	-mkdir -p /home/actility/workspace/song.binding.coap/src/main/resources/lib/linux-i686
	cp libdiajni.so /home/actility/workspace/song.binding.coap/src/main/resources/lib/linux-i686

clean :
	@(rm *.x *.o *.a *.so *.dylib core *.class diajni.h 2>/dev/null || exit 0)
