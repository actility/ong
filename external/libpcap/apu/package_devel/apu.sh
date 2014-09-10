
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

  cd libpcap
  tar zxvf libpcap-1.3.0.tar.gz
  cd libpcap-1.3.0/
  if [ ! -f Makefile ]; then
    if [ $CROSS_COMPILATION = 1 ]; then
        if [ "x" != "x$FLAGS" ]; then
            CFLAGS="$FLAGS" LDFLAGS="$FLAGS" ./configure --with-pcap=linux --host $PREFIX --prefix $ROOTACT
        else
            ./configure --with-pcap=linux --host $PREFIX --prefix $ROOTACT
        fi
    else
        ./configure --with-pcap=linux --prefix $ROOTACT
    fi
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
 
  PCAP_VERSION=`projectVersion`

  cp -r include apu/data/
  cp -r lib apu/data/ 
  cp -r bin apu/data/

  cp libpcap/apu/$1/postinst apu/control/postinst 
}

projectGroupId()
{
  echo "org.tcpdump"
}

projectName()
{
  echo "libpcap-devel"
}

projectVersion()
{
  echo "1.3.0"
}

projectApuRev()
{
  cat libpcap/apu/revision
}

projectDescription()
{
  echo "Library for user-level packet capture."
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}


