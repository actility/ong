SUBDIRS=include lib supervisor xoref tests

ifndef ROOTACT 
$(error ROOTACT not defined)
endif

all clean: $(SUBDIRS)
all: TARGET=all
clean: TARGET=clean

$(SUBDIRS): force
	$(MAKE) -C $@ $(TARGET)

#	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done
	
#run:
#	mkdir -p runtime-data
#	$(MAKE) -C supervisor run

## Ensure doxygen and graphviz are installed
doc: 
	sed -i -e "s/PROJECT_NUMBER.*=.*/PROJECT_NUMBER = 0.0.1/" doc/knx.Doxyfile
	doxygen doc/knx.Doxyfile

#rootfs:
#	rm -Rf rootfs
#	mkdir -p rootfs rootfs/bin \
#	                rootfs/lib \
#	                rootfs/etc/init.d \
#	                rootfs/etc/knx/diatemplates \
#	                rootfs/usr/etc/knx/diatemplates \
#	                rootfs/usr/data/knx/networks \
#	                rootfs/var/log/knx
#	                
#	                
#	#
#	# Install eibd
#	#
#	cp contrib/root/bin/eibd rootfs/bin/eibd
#	cp contrib/root/lib/*.so rootfs/lib/
#	ln -sf ./libpthsem.so rootfs/lib/libpthsem.so.20
#	ln -sf ./libpthsem.so rootfs/lib/libpthsem.so.20.0.28
#	ln -sf ./libeibclient.so rootfs/lib/libeibclient.so.0
#	ln -sf ./libeibclient.so rootfs/lib/libeibclient.so.0.0.0
#	
#	#
#	# Install KNX
#	#
#	cp config/dpt.xml rootfs/usr/data/knx/dpt.xml
#	cp config/manufacturer.xml rootfs/usr/data/knx/manufacturer.xml
#	cp config/ranges.xml rootfs/usr/data/knx/ranges.xml
#	cp config/spvconfig.xml rootfs/usr/etc/knx/spvconfig.xml
#	cp config/modelconfig.xml rootfs/usr/etc/knx/modelconfig.xml
#	cp config/namespace.xns rootfs/etc/knx/namespace.xns
#	cp supervisor/knx.x rootfs/bin/knx.x
#	cp diatemplates/*.xml rootfs/usr/etc/knx/diatemplates/
#	cp xoref/knx.xor rootfs/etc/knx/knx.xor
#	cp knx-init rootfs/etc/init.d/knx
#
#install: rootfs
#install:
#	cd rootfs && scp -r . root@$(ONG_HOSTNAME):/home/ong/

.PHONY: doc force
#.PHONY: doc rootfs install
.NOTPARALLEL:

force:;
