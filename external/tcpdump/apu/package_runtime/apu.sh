
applyPatch() 
{
  echo -n "Apply $1..."
  patch -p1 -i $1 || (cd - && echo "FAILED" && exit 1)
  echo "OK"
}

buildProject()
{
  ROOTACT=`pwd`
  FLAGS=
  case "$1" in 
    centos6-x86)
      CROSS_COMPILATION=0
      ;;
    cov1)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-linux
      ;;
    cov2)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-none-linux-gnueabi
      ;;
    lpv3)
      CROSS_COMPILATION=1
      PREFIX=mips-openwrt-linux-uclibc
      ;;
    rpib)
      CROSS_COMPILATION=1
      source $RPI_B_HOST_HOME/../../environment-setup-armv6-vfp-poky-linux-gnueabi
      PREFIX=arm-poky-linux-gnueabi
      ;;
    *)
      ;;
  esac

  cd tcpdump
  tar zxvf tcpdump-4.3.0.tar.gz
  cd tcpdump-4.3.0/

  export TARGET_DIR=$ROOTACT
  export PATH=$PATH:$ROOTACT/bin # this is required to locate pcap-config

  echo ">>> TARGET_DIR=$TARGET_DIR" 

  # apply patch
  sed -i 's:-L/lib:-L${TARGET_DIR}/lib:g' ./configure.in
  if [[ $1 != centos6-x86 ]]
  then
    applyPatch $ROOTACT/tcpdump/patches/configure.patch
    applyPatch $ROOTACT/tcpdump/patches/tcpdump_configure_no_-O2.patch
  fi

  autoreconf
  if [ ! -f Makefile ]; then
    if [ $CROSS_COMPILATION = 1 ]; then
        if [ "x" != "x$FLAGS" ]; then
            ac_cv_linux_vers=2 CFLAGS="$FLAGS" LDFLAGS="$FLAGS" ./configure --host $PREFIX --prefix $TARGET_DIR || (echo "FAILED to configure" && exit 1)
        else
            ac_cv_linux_vers=2 ./configure --host $PREFIX --prefix $TARGET_DIR || (echo "FAILED to configure" && exit 1)
        fi
    else
        ./configure --prefix $TARGET_DIR || (echo "FAILED to configure" && exit 1)
    fi
  fi
  sed -i 's:/usr/lib:${TARGET_DIR}/lib:' ./Makefile
  sed -i 's:/usr/include:${TARGET_DIR}/include:' ./Makefile
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/control
 
  cp -r sbin/tcpdump apu/data/bin

  cp tcpdump/apu/$1/postinst apu/control/postinst
}

projectGroupId()
{
  echo "org.tcpdump"
}

projectName()
{
  echo "tcpdump"
}

projectVersion()
{
  echo "4.3.0"
}

projectApuRev()
{
  cat tcpdump/apu/revision
}

projectDescription()
{
  echo "tcpdump is a command-line packet analyzer"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

