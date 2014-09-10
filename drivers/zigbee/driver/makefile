
ROOTACT ?= $(cd ../..; pwd) 

################################################################################
include $(ROOTACT)/rtbase/base/makefile.def
include $(ROOTACT)/zigbee/base/makefile.def

################################################################################
LIB = \
$(LIBAZAP)

SHLIB = \
-L$(ROOTACT)/shlib -ldIa -lrtbase -lxo -ldrvcommon

INC = \
-I. \
-I$(ROOTACT)/drvcommon/lib \
-I$(ROOTACT)/AZAP/include \
-I$(ROOTACT)/rtbase/include \
-I$(ROOTACT)/xo/include \
-I$(ROOTACT)/libdIa \
-I$(ROOTACT)/libcoap

EXTRALD = -lm
ifeq ($(SYSTEM),cov1)
EXTRALD += -lpthread
endif


################################################################################
EXE = azbspv

SRC = \
admcli.c \
azap_listener.c \
interface_mapping_desc.c \
version.c \
zigbee_node.c \
zigbee_han.c \
sensor_impl.c \
var_impl.c \
dia_impl.c \
m2m_abstraction.c \
m2m_zdo_abstraction.c \
m2m_zcl_abstraction.c \
m2m_zcl_0x0000_abstraction.c \
m2m_zcl_0x0003_abstraction.c \
m2m_zcl_0x0006_abstraction.c \
m2m_zcl_0x0400_abstraction.c \
m2m_zcl_0x0402_abstraction.c \
m2m_zcl_0x0403_abstraction.c \
m2m_zcl_0x0404_abstraction.c \
m2m_zcl_0x0405_abstraction.c \
m2m_zcl_0x0702_abstraction.c \
driver_req_context.c \
main.c

OBJ = $(SRC:.c=.o)


################################################################################
all : $(EXE).x

$(EXE).x : _whatstr.h $(OBJ) $(LIB)
	@(echo -e "$(BLINK)>>> link $@$(NORM)")
	$(CC) $(STATIC) $(LDFLAGS) $(XML2LD) -o $@ $^ $(EXTRALD) $(LIBXML2) $(SHLIB)

_whatstr.h : ../Version
	$(MKWHAT) -p zigbee -vf ../Version > _whatstr.h

#%.o : %.c ../include/_whatstr.h
%.o	: %.c $(foreach file,$(shell ls -1 *.h),$(file))
	@(echo -e "$(BLINK)>>> compile $<$(NORM)")
	$(CC) $(CCFLAGS) $(INC) $(DEFINC) -c $<

clean :
	@(echo -e "$(BLINK)>>> clean$(NORM)")
	@(rm -f *.x *.o *.a core *.log _whatstr.h 2>/dev/null || exit 0)

info :
	@(echo -e "$(BLINK)>>> info$(NORM)")
	@(echo $(EXE).x)


