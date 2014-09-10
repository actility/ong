ONG_HOSTNAME=co2
SUBDIRS=lib supervisor xoref tests

ifndef ROOTACT 
$(error ROOTACT not defined)
endif

all:
%:
	$(MAKE) -C supervisor cproto.h
	$(MAKE) -C lib libmodbus-acy.h
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@ || exit; done
	
run:
	mkdir -p runtime-data
	$(MAKE) -C supervisor run

package:
	git archive --format tar HEAD | gzip > modbus-`cat Version`.tar.gz
	
## Ensure doxygen and graphviz are installed
doc:
	mkdir -p doc
	sed -i -e "s/PROJECT_NUMBER.*=.*/PROJECT_NUMBER = `cat Version`/" modbus.Doxyfile
	doxygen modbus.Doxyfile

rootfs:
	rm -Rf rootfs
	mkdir -p rootfs rootfs/bin \
	                rootfs/lib \
	                rootfs/etc/init.d \
	                rootfs/etc/modbus/diatemplates \
	                rootfs/usr/etc/modbus/diatemplates \
	                rootfs/usr/data/modbus/networks \
	                rootfs/var/log/modbus
	#
	# Install Modbus
	#
	cp config/spvconfig.xml rootfs/usr/etc/modbus/spvconfig.xml
	cp config/modelconfig.xml rootfs/usr/etc/modbus/modelconfig.xml
	cp config/namespace.xns rootfs/etc/modbus/namespace.xns
	cp supervisor/modbus.x rootfs/bin/modbus.x
	cp diatemplates/*.xml rootfs/usr/etc/modbus/diatemplates/
	cp xoref/modbus.xor rootfs/etc/modbus/modbus.xor
	cp modbus-init rootfs/etc/init.d/modbus

install: rootfs
install:
	cd rootfs && scp -r . root@$(ONG_HOSTNAME):/home/ong/

.PHONY: doc rootfs install
.NOTPARALLEL:
