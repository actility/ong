include $(ROOTACT)/rtbase/base/makefile.def

SONG_BINDING_COAP=${ROOTACT}/workspace/song.binding.coap

JAVAHOME=/System/Library/Frameworks/JavaVM.framework/Versions/Current
JAVAINC=${JAVAHOME}/Headers
CFLAGS+=-fPIC -I${JAVAINC} -I.. -I$(ROOTACT)/rtbase/include -I$(ROOTACT)/libcoap
LDFLAGS+=-L$(ROOTACT)/shlib
LIBS=-ldIa -lrtbase
JAVAFLAGS=-target 1.3 -source 1.3

all :
	if test -d ${SONG_BINDING_COAP}; then make all2; else exit 0; fi

all2 : DiaJni.class libdiajni.so

libdiajni.so : diajni.c com_actility_servlet_song_coap_DiaJni.h
	$(CC) $(CFLAGS) -shared diajni.c -o libdiajni.so $(LDFLAGS) $(LIBS)
	-mkdir -p ${SONG_BINDING_COAP}/src/main/resources/lib/linux-i686
	cp libdiajni.so ${SONG_BINDING_COAP}/src/main/resources/lib/linux-i686
	cp libdiajni.so ${SONG_BINDING_COAP}/target/classes/lib/linux-i686
	./COMPILE

com_actility_servlet_song_coap_DiaJni.h : DiaJni.java
	-rm com_actility_servlet_song_coap_DiaJni.h
	javah -classpath ${SONG_BINDING_COAP}/target/classes/ -jni com.actility.servlet.song.coap.DiaJni

DiaJni.class : DiaJni.java DiaOpt.java SongCoAPBinding.java osgi/SongCoAPBindingActivator.java
	cp *.java ${SONG_BINDING_COAP}/src/main/java/com/actility/servlet/song/coap
	cp -r osgi ${SONG_BINDING_COAP}/src/main/java/com/actility/servlet/song/coap
	./COMPILE
	touch DiaJni.class

restart : all
	./RESTART

clean :
	@(rm *.x *.o *.a *.so *.dylib core *.class com_actility_servlet_song_coap_DiaJni.h 2>/dev/null || exit 0)
