################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/drvcommon/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
INCLUDE=-I../include -I$(ROOTACT)/xo/include\
	-I$(ROOTACT)/libdIa -I$(ROOTACT)/libcoap $(DEFINC)

################################################################################
LIB=libdrvcommon.a
LIBSO=libdrvcommon.so
PROF=
OBJLIB=\
	$(LIB)(diaclient.o)\
	$(LIB)(diaserver.o)\
	$(LIB)(diatools.o)\
	$(LIB)(diakal.o)\
	$(LIB)(diaipu.o)\
	$(LIB)(dianet.o)\
	$(LIB)(diadev.o)\
	$(LIB)(diaapp.o)\
	$(LIB)(diadat.o)\
	$(LIB)(diarep.o)\
	$(LIB)(diaacc.o)\
	$(LIB)(admtcp.o)\
	$(LIB)(ctools.o)\
	$(LIB)(xmltools.o)\
	$(LIB)(model.o)\
	$(LIB)(vartools.o)\
	$(LIB)(iso8601.o)
################################################################################
.PRECIOUS : $(LIB)

all	: _whatstr.h $(OBJLIB) $(LIBSO)

mbr	:

clean	:
	@(rm *.a *.so *.o _whatstr.h 2>/dev/null || exit 0)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)

_whatstr.h: ../Version
	$(MKWHAT) -p drvcommon -vf ../Version > _whatstr.h

.c.a :
	$(CC) $(CFLAGS) $(LIBCFLAGS) $(CFULLWARN) $(INCLUDE) $(PROF) -c $*.c
	@$(AR) $(ARFLAGS) $(LIB) $*.o
	@$(RM) $*.o

$(LIBSO) : $(OBJLIB)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)
	@$(RM) *.o
	$(AR) -x $(LIB)
	$(LD) $(LDSOFLAGS) -o $(LIBSO) *.o
	@$(RM) *.o
	$(MKSHLIB) -p drvcommon -l lib/$(LIBSO)
