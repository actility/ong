################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
INC=-I../include

################################################################################
LIB=librtbase.a
LIBSO=librtbase.so


OBJLIB=\
	$(LIB)(trace.o)\
	$(LIB)(poll.o)\
	$(LIB)(message.o)\
	$(LIB)(hashtbl.o)\
	$(LIB)(basename.o)\
	$(LIB)(tools.o)\
	$(LIB)(dir.o)\
	$(LIB)(mkdirp.o)\
	$(LIB)(timems.o)\
	$(LIB)(timemsmono.o)\
	$(LIB)(dolpath.o)\
	$(LIB)(dolpath2.o)\
	$(LIB)(base64.o)\
	$(LIB)(iso8601.o)\
	$(LIB)(ini.o)\
	$(LIB)(inihashvar.o)\
	$(LIB)(vasprintf.o)\
	$(LIB)(version.o)

################################################################################

all	: $(OBJLIB) $(LIBSO)

$(LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)

version.o	: version.c ../include/_whatstr.h
	$(CC) $(CCFLAGS) $(INC) -c $<

#%.o	: %.c
#	$(CC) $(CCFLAGS) $(INC) -c $<

.c.a :
	$(CC) $(CFLAGS) $(LIBCFLAGS) $(CFULLWARN) $(INC) -c $*.c
	@$(AR) $(ARFLAGS) $(LIB) $*.o
	@$(RM) $*.o

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
	@$(RM) $(LIBSO)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)

$(LIBSO) : $(OBJLIB)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)
	@$(RM) *.o
	$(AR) -x $(LIB)
	$(LD) $(LDSOFLAGS) -Wl,-soname=$(LIBSO) -o $(LIBSO) *.o
	@$(RM) *.o
	$(MKSHLIB) -p rtbase -l lib/$(LIBSO)
