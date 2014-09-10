
buildProject()
{
  ROOTACT=`pwd`
  FLAGS=
  CONFIGURE_OPTIONS="--enable-onlyeibd --enable-eibnetip --enable-eibnetiptunnel --enable-eibnetipserver --without-libstdc --with-pth=$ROOTACT --prefix $ROOTACT"
  case "$1" in 
    centos6-x86)
      CROSS_COMPILATION=0
      CONFIGURE_ADD_OPTIONS=
      ;;
    cov1)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-linux
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      ;;
    cov2)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-none-linux-gnueabi
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      ;;
    lpv3)
      CROSS_COMPILATION=1
      PREFIX=mips-openwrt-linux-uclibc
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      FLAGS="-I$ROOTACT/include"
      CONFIGURE_ADD_OPTIONS=$CONFIGURE_ADD_OPTIONS" LIBS=$ROOTACT/lib/libargp.a"
      ;;
    rpib)
      CROSS_COMPILATION=1
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      source $RPI_B_HOST_HOME/../../environment-setup-armv6-vfp-poky-linux-gnueabi
      PREFIX=arm-poky-linux-gnueabi
      ;;
    *)
      ;;
  esac

  cd eibd
  tar zxvf bcusdk_0.0.5.tar.gz
  cd bcusdk-0.0.5/
  export LD_LIBRARY_PATH=$ROOTACT/lib
  
  if [ ! -f Makefile ]; then
    if [ $CROSS_COMPILATION = 1 ]; then
        if [ "x" != "x$FLAGS" ] ; then
            CFLAGS="$FLAGS" LDFLAGS="$FLAGS" ./configure --host $PREFIX $CONFIGURE_OPTIONS $CONFIGURE_ADD_OPTIONS
        else
            ./configure --host $PREFIX $CONFIGURE_OPTIONS $CONFIGURE_ADD_OPTIONS
        fi
    else
        ./configure $CONFIGURE_OPTIONS $CONFIGURE_ADD_OPTIONS
    fi
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/lib && \
  mkdir -p apu/data/bin && \
  mkdir -p apu/control && \
  EIBD_VERSION="0.0.0" && \
  cp lib/libeibclient.so.$EIBD_VERSION apu/data/lib && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -s libeibclient.so.$EIBD_VERSION libeibclient.so" >> apu/control/postinst && \
  echo "ln -s libeibclient.so.$EIBD_VERSION libeibclient.so.$(echo $EIBD_VERSION | cut -d.  -f1)" >> apu/control/postinst && \
  cp bin/bcuaddrtab apu/data/bin && \
  cp bin/bcuread apu/data/bin && \
  cp bin/busmonitor1 apu/data/bin && \
  cp bin/busmonitor2 apu/data/bin && \
  cp bin/eibd apu/data/bin && \
  cp bin/eibnetdescribe apu/data/bin && \
  cp bin/eibnetsearch apu/data/bin && \
  cp bin/groupcacheclear apu/data/bin && \
  cp bin/groupcachedisable apu/data/bin && \
  cp bin/groupcacheenable apu/data/bin && \
  cp bin/groupcachelastupdates apu/data/bin && \
  cp bin/groupcacheread apu/data/bin && \
  cp bin/groupcachereadsync apu/data/bin && \
  cp bin/groupcacheremove apu/data/bin && \
  cp bin/grouplisten apu/data/bin && \
  cp bin/groupread apu/data/bin && \
  cp bin/groupreadresponse apu/data/bin && \
  cp bin/groupresponse apu/data/bin && \
  cp bin/groupsocketlisten apu/data/bin && \
  cp bin/groupsocketread apu/data/bin && \
  cp bin/groupsocketswrite apu/data/bin && \
  cp bin/groupsocketwrite apu/data/bin && \
  cp bin/groupsresponse apu/data/bin && \
  cp bin/groupswrite apu/data/bin && \
  cp bin/groupwrite apu/data/bin && \
  cp bin/knxtool apu/data/bin && \
  cp bin/madcread apu/data/bin && \
  cp bin/maskver apu/data/bin && \
  cp bin/mmaskver apu/data/bin && \
  cp bin/mpeitype apu/data/bin && \
  cp bin/mprogmodeoff apu/data/bin && \
  cp bin/mprogmodeon apu/data/bin && \
  cp bin/mprogmodestatus apu/data/bin && \
  cp bin/mprogmodetoggle apu/data/bin && \
  cp bin/mpropdesc apu/data/bin && \
  cp bin/mpropread apu/data/bin && \
  cp bin/mpropscan apu/data/bin && \
  cp bin/mpropscanpoll apu/data/bin && \
  cp bin/mpropwrite apu/data/bin && \
  cp bin/mread apu/data/bin && \
  cp bin/mrestart apu/data/bin && \
  cp bin/msetkey apu/data/bin && \
  cp bin/mwrite apu/data/bin && \
  cp bin/mwriteplain apu/data/bin && \
  cp bin/progmodeoff apu/data/bin && \
  cp bin/progmodeon apu/data/bin && \
  cp bin/progmodestatus apu/data/bin && \
  cp bin/progmodetoggle apu/data/bin && \
  cp bin/readindividual apu/data/bin && \
  cp bin/vbusmonitor1 apu/data/bin && \
  cp bin/vbusmonitor1poll apu/data/bin && \
  cp bin/vbusmonitor2 apu/data/bin && \
  cp bin/writeaddress apu/data/bin && \
  cp bin/xpropread apu/data/bin && \
  cp bin/xpropwrite apu/data/bin && \
  echo "chmod +x \$ROOTACT/bin/bcuaddrtab" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/bcuaddrtab" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/bcuread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/busmonitor1" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/busmonitor2" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/eibd" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/eibnetdescribe" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/eibnetsearch" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcacheclear" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcachedisable" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcacheenable" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcachelastupdates" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcacheread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcachereadsync" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupcacheremove" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/grouplisten" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupreadresponse" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupresponse" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupsocketlisten" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupsocketread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupsocketswrite" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupsocketwrite" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupsresponse" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupswrite" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/groupwrite" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/knxtool" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/madcread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/maskver" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mmaskver" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mpeitype" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mprogmodeoff" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mprogmodeon" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mprogmodestatus" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mprogmodetoggle" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mpropdesc" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mpropread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mpropscan" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mpropscanpoll" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mpropwrite" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mrestart" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/msetkey" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mwrite" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/mwriteplain" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/progmodeoff" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/progmodeon" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/progmodestatus" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/progmodetoggle" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/readindividual" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/vbusmonitor1" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/vbusmonitor1poll" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/vbusmonitor2" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/writeaddress" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/xpropread" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/bin/xpropwrite" >> apu/control/postinst
}

projectGroupId()
{
  echo "org.gnu"
}

projectName()
{
  echo "eibd"
}

projectVersion()
{
  echo "0.0.5"
}

projectApuRev()
{
  cat eibd/apu/revision
}

projectDescription()
{
  echo "EIB daemon for the programming of the EIB buscouplers (BCU)"
}

projectArchDepend()
{
  # yes, need arch for building runtime apu
  return 0
}


