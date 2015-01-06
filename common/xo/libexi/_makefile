
#################################################
#						#
#	Makefile de la librairie xo		#
#						#
#################################################

include $(ROOTACT)/rtbase/base/makefile.def

LIB	=libxoexi.a
LIBSO	=libxoexi.so
LIBEXI	=$(ROOTACT)/exip/bin/lib/libexip.a

#ifneq ($(wildcard ./_doexi),)

ifneq ($(wildcard $(ROOTACT)/exip),)

INCLUDE=-I../include -I$(XML2INC) -I$(ROOTACT)/rtbase/include
INCLUDE += -I$(ROOTACT)/exip/src/grammar/include -I$(ROOTACT)/exip/include -I$(ROOTACT)/exip/build/gcc/pc -I$(ROOTACT)/exip/src/stringTables/include
CFLAGS += -DXO_WITH_EXIP


#PROF=-p
PROF=

OBJLIB=\
	$(LIB)(apitab.o)\
	$(LIB)(api.o)\
	$(LIB)(apichang.o)\
	$(LIB)(apiinfo.o)\
	$(LIB)(apidump.o)\
	$(LIB)(apisave.o)\
	$(LIB)(apiload.o)\
	$(LIB)(apiascii.o)\
	$(LIB)(apicompr.o)\
	$(LIB)(apiwalk.o)\
	$(LIB)(apiexter.o)\
	$(LIB)(apiexten.o)\
	$(LIB)(apiXXXread.o)\
	$(LIB)(apixmlns.o)\
	$(LIB)(apixmlread.o)\
	$(LIB)(apixmlwrit.o)\
	$(LIB)(apixmlsave.o)\
	$(LIB)(apixmlobix.o)\
	$(LIB)(apijsowrit.o)\
	$(LIB)(apidir.o)\
	$(LIB)(apishareddico.o)\
	$(LIB)(apilz.o)\
	$(LIB)(apisection.o)\
	$(LIB)(apiexiwrit.o)\
	$(LIB)(apiexischema.o)\
	$(LIB)(apiexiread.o)

.PRECIOUS : $(LIB)

all	: _whatstr.h $(OBJLIB) $(LIBSO)

mbr	:

clean	:
	@(rm *.a *.so *.o _whatstr.h 2>/dev/null || exit 0)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)

_whatstr.h: ../Version
	$(MKWHAT) -p xo -vf ../Version > _whatstr.h

%.o : ../lib/%.c
	$(CC) $(CFLAGS) $(LIBCFLAGS) $(CFULLWARN) $(INCLUDE) $(PROF) -c ../lib/$*.c
	@$(AR) $(ARFLAGS) $(LIB) $*.o

#	$(LD) $(LDSOFLAGS) -lxml2 -Wl,-soname=$(LIBSO) -o $(LIBSO) *.o

$(LIBSO) : $(OBJLIB)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)
	@$(RM) *.o
	$(AR) -x $(LIB)
	$(LD) $(LDSOFLAGS) -Wl,-soname=$(LIBSO) -o $(LIBSO) *.o $(LIBEXI) -lxml2
	@$(RM) *.o
	$(MKSHLIB) -p xo -l libexi/$(LIBSO)
else
all	: info

clean	:
	@(rm *.a *.so *.o _whatstr.h 2>/dev/null || exit 0)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)

info	:
	echo	"no exip found in $(ROOTACT)"
endif
