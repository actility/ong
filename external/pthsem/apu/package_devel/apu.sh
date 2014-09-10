
buildProject()
{
  ROOTACT=`pwd`
  FLAGS=
  case "$1" in 
    centos6-x86)
      CROSS_COMPILATION=0
      CONFIGURE_ADD_OPTIONS=
      ;;
    cov1)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-linux
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    cov2)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-none-linux-gnueabi
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    lpv3)
      CROSS_COMPILATION=1
      PREFIX=mips-openwrt-linux-uclibc
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    rpib)
      CROSS_COMPILATION=1
      source $RPI_B_HOST_HOME/../../environment-setup-armv6-vfp-poky-linux-gnueabi
      PREFIX=arm-poky-linux-gnueabi
      ;;
    *)
      ;;
  esac

  cd pthsem
  tar zxvf pthsem_2.0.8.tar.gz
  cd pthsem-2.0.8/
  if [ ! -f Makefile ]; then
    if [ $CROSS_COMPILATION = 1 ]; then
        if [ "x" != "x$FLAGS" ]; then
            CFLAGS="$FLAGS" LDFLAGS="$FLAGS" ./configure --host $PREFIX --prefix $ROOTACT
        else
            ./configure --host $PREFIX --prefix $ROOTACT
        fi
    else
        ./configure --prefix $ROOTACT
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

  cp pthsem/apu/$1/postinst apu/control/postinst
}

projectGroupId()
{
  echo "org.gnu"
}

projectName()
{
  echo "pthsem-devel"
}

projectVersion()
{
  echo "2.0.8"
}

projectApuRev()
{
  cat pthsem/apu/revision
}

projectDescription()
{
  echo "Library for for threading management and semaphore."
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}


