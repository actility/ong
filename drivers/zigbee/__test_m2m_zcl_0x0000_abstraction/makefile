################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/zigbee/base/makefile.def

LIBBASE=$(ROOTACT)/rtbase/lib/librtbase.a
LIBDIA=$(ROOTACT)/libdIa/libdIa.a
LIBCOAP=$(ROOTACT)/libcoap/libcoap.a
LIBXO=$(ROOTACT)/xo/lib/libxo.a

################################################################################
#LIB=$(PRJLIB)
LIB=

SHLIB = \
-L$(ROOTACT)/shlib -ldIa -lrtbase -lxo -ldrvcommon

STATIC=\
../driver/interface_mapping_desc.o \
../driver/version.o \
../driver/zigbee_node.o \
../driver/zigbee_han.o \
../driver/sensor_impl.o \
../driver/var_impl.o \
../driver/dia_impl.o \
../driver/m2m_abstraction.o \
../driver/m2m_zdo_abstraction.o \
../driver/m2m_zcl_abstraction.o \
../driver/m2m_zcl_0x0000_abstraction.o \
../driver/m2m_zcl_0x0003_abstraction.o \
../driver/m2m_zcl_0x0006_abstraction.o \
../driver/m2m_zcl_0x0400_abstraction.o \
../driver/m2m_zcl_0x0402_abstraction.o \
../driver/m2m_zcl_0x0403_abstraction.o \
../driver/m2m_zcl_0x0404_abstraction.o \
../driver/m2m_zcl_0x0405_abstraction.o \
../driver/m2m_zcl_0x0702_abstraction.o \
../driver/driver_req_context.o \
$(ROOTACT)/AZAP/lib/libazap.a

#../../AZAP/lib/azap_tools.o \
#../../AZAP/lib/azap_endpoint.o \
#../../AZAP/lib/azap_attribute.o \
#../../AZAP/lib/azap_zcl_attr_val.o

#../driver/admcli.o 
#../driver/azap_listener.o 
INC=-I../../AZAP/include -I../driver/ -I../../drvcommon/lib

CFLAGS+=-g -O0

################################################################################
EXE=m2m_zcl_0x0000_abstraction_test

SRC=\
m2m_zcl_0x0000_abstraction_test.c

OBJ=$(SRC:.c=.o)

################################################################################
all	: $(EXE).x

$(EXE).x : $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@ $(NORM)")
	$(CC) $(SHLIB) $(STATIC) $(LDFLAGS) $(XML2LD) -lm -o $@ $^ 

%.o	: %.c
	@(echo -e "$(BLINK)>>> compilation $< $(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean	:
	@(echo -e "$(BLINK)>>> $@ $(NORM)")
	@(rm -f *.x *.o *.a core *.log 2>/dev/null || exit 0)

info	:
	@(echo $(EXE).x)
