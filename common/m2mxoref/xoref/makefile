################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

################################################################################
XML2_INCLUDEDIR=-I$(XML2INC)
#XML2_LIBS=$(XML2LIB)
XML2_LIBS=
LDFLAGS += -lxml2
################################################################################
LIB=$(DEFLIB)
LIBXO=$(ROOTACT)/xo/lib/libxo.a
INC=-I$(ROOTACT)/xo/include $(XML2_INCLUDEDIR)


################################################################################
EXE=
#VERBOSE=-v -g
UPPERCASE=-U
COMPXO=$(ROOTACT)/xo/com/MKXO.sh $(VERBOSE) -p -f
LINKXO=$(ROOTACT)/xo/com/xold.x

################################################################################

M2MXOR=actility.xor accessRightAnnc.xor accessRight.xor accessRights.xor applicationAnnc.xor application.xor applications.xor containerAnnc.xor container.xor containers.xor contentInstance.xor contentInstances.xor discovery.xor errorInfo.xor groupAnnc.xor group.xor groups.xor locationContainerAnnc.xor locationContainer.xor m2mPoc.xor m2mPocs.xor membersContent.xor modeling.xor notificationChannel.xor notificationChannels.xor notify.xor responseNotify.xor sclBase.xor scl.xor scls.xor subscription.xor subscriptions.xor


all	: $(M2MXOR) xobix.xor m2m.xor 

xobix.xor		: xobix.ref
	$(ROOTACT)/xo/com/mkxo.x $(VERBOSE) $(UPPERCASE) -f xobix

actility.xor	: actility.ref misc.ref common.ref extern.ref
	$(COMPXO) actility

accessRightAnnc.xor	: accessRightAnnc.ref misc.ref common.ref extern.ref
	$(COMPXO) accessRightAnnc

accessRight.xor	: accessRight.ref misc.ref common.ref extern.ref
	$(COMPXO) accessRight

accessRights.xor	: accessRights.ref misc.ref common.ref extern.ref
	$(COMPXO) accessRights

applicationAnnc.xor	: applicationAnnc.ref misc.ref common.ref extern.ref
	$(COMPXO) applicationAnnc

application.xor	: application.ref misc.ref common.ref extern.ref
	$(COMPXO) application

applications.xor	: applications.ref misc.ref common.ref extern.ref
	$(COMPXO) applications

common.xor	: common.ref misc.ref common.ref extern.ref
	$(COMPXO) common

containerAnnc.xor	: containerAnnc.ref misc.ref common.ref extern.ref
	$(COMPXO) containerAnnc

container.xor	: container.ref misc.ref common.ref extern.ref
	$(COMPXO) container

containers.xor	: containers.ref misc.ref common.ref extern.ref
	$(COMPXO) containers

contentInstance.xor	: contentInstance.ref misc.ref common.ref extern.ref
	$(COMPXO) contentInstance

contentInstances.xor	: contentInstances.ref misc.ref common.ref extern.ref
	$(COMPXO) contentInstances

discovery.xor	: discovery.ref misc.ref common.ref extern.ref
	$(COMPXO) discovery

errorInfo.xor	: errorInfo.ref misc.ref common.ref extern.ref
	$(COMPXO) errorInfo

extern.xor	: extern.ref misc.ref common.ref extern.ref
	$(COMPXO) extern

groupAnnc.xor	: groupAnnc.ref misc.ref common.ref extern.ref
	$(COMPXO) groupAnnc

group.xor	: group.ref misc.ref common.ref extern.ref
	$(COMPXO) group

groups.xor	: groups.ref misc.ref common.ref extern.ref
	$(COMPXO) groups

locationContainerAnnc.xor	: locationContainerAnnc.ref misc.ref common.ref extern.ref
	$(COMPXO) locationContainerAnnc

locationContainer.xor	: locationContainer.ref misc.ref common.ref extern.ref
	$(COMPXO) locationContainer

m2mPoc.xor	: m2mPoc.ref misc.ref common.ref extern.ref
	$(COMPXO) m2mPoc

m2mPocs.xor	: m2mPocs.ref misc.ref common.ref extern.ref
	$(COMPXO) m2mPocs

membersContent.xor	: membersContent.ref misc.ref common.ref extern.ref
	$(COMPXO) membersContent

misc.xor	: misc.ref misc.ref common.ref extern.ref
	$(COMPXO) misc

modeling.xor	: modeling.ref misc.ref common.ref extern.ref
	$(COMPXO) modeling

notificationChannel.xor	: notificationChannel.ref misc.ref common.ref extern.ref
	$(COMPXO) notificationChannel

notificationChannels.xor	: notificationChannels.ref misc.ref common.ref extern.ref
	$(COMPXO) notificationChannels

notify.xor	: notify.ref misc.ref common.ref extern.ref
	$(COMPXO) notify

responseNotify.xor	: responseNotify.ref misc.ref common.ref extern.ref
	$(COMPXO) responseNotify

sclBase.xor	: sclBase.ref misc.ref common.ref extern.ref
	$(COMPXO) sclBase

scl.xor	: scl.ref misc.ref common.ref extern.ref
	$(COMPXO) scl

scls.xor	: scls.ref misc.ref common.ref extern.ref
	$(COMPXO) scls

subscription.xor	: subscription.ref misc.ref common.ref extern.ref
	$(COMPXO) subscription

subscriptions.xor	: subscriptions.ref misc.ref common.ref extern.ref
	$(COMPXO) subscriptions

m2m.xor	: $(M2MXOR) xobix.xor
	$(LINKXO) -o m2m.xor $(M2MXOR) xobix.xor

clean	:
	@(rm *.x *.o *.a *.xor *.i def*.h core 2>/dev/null || exit 0)
