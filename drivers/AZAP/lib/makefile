################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/AZAP/makefile.def

################################################################################
#LIB=$(PRJLIB)
INC=-I../include -I. -I$(ROOTACT)/rtbase/include/

################################################################################
LIB=libazap
#	af_msg_context.c 
SRC=\
  postponed_request_elts.c \
  request_context.c \
  request_manager.c \
	azap_attribute.c \
	azap_endpoint.c \
	port_context.c \
	format_znp.c \
	format_znp_sys.c \
	format_znp_zdo.c \
	format_znp_af.c \
	znp_start_sm.c \
	azap_tools.c \
	uart_buf_control.c \
	azap_api.c \
  azap_zcl_attr_val.c \
	azap_api_impl.c \
	version.c

OBJ=$(SRC:.c=.o)
################################################################################

all	: $(LIB).a

$(LIB).a : $(OBJ)
	@(echo -e "$(BLINK)>>> link $@ $(NORM)")
	$(AR) $(ARFLAGS) $(LIB).a $(OBJ)

version.o	: version.c ../include/_whatstr.h
	@(echo -e "$(BLINK)>>> compilation version$(NORM)")
	$(CC) $(CCFLAGS) $(INC) -c $<

%.o	: %.c
	@(echo -e "$(BLINK)>>> compilation $< $(NORM)")
	$(CC) $(CCFLAGS) $(INC) -c $<

clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core 2>/dev/null || exit 0)

