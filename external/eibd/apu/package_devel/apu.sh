
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
  mkdir -p apu/control && \
  PCAP_VERSION=`projectVersion` && \
  cp -r include apu/data/ && \
  cp lib/libeibclient.* apu/data/lib/ && \
  cp -r bin apu/data/

}

projectGroupId()
{
  echo "org.gnu"
}

projectName()
{
  echo "eibd-devel"
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
  # yes, need arch for building devel apu
  return 0
}


