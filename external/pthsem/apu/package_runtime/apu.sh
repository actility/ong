
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
            CFLAGS="$FLAGS" LDFLAGS="$FLAGS" ./configure --host $PREFIX --prefix $ROOTACT $CONFIGURE_ADD_OPTIONS
        else
            ./configure --host $PREFIX --prefix $ROOTACT $CONFIGURE_ADD_OPTIONS
        fi
    else
        ./configure --prefix $ROOTACT $CONFIGURE_ADD_OPTIONS
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
  PTHSEM_VERSION="20.0.28" && \
  cp -r lib/libpthsem.so.$PTHSEM_VERSION apu/data/lib/ && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -s libpthsem.so.$PTHSEM_VERSION libpthsem.so" >> apu/control/postinst && \
  echo "ln -s libpthsem.so.$PTHSEM_VERSION libpthsem.so.$(echo $PTHSEM_VERSION | cut -d. -f1)" >> apu/control/postinst
}

projectGroupId()
{
  echo "org.gnu"
}

projectName()
{
  echo "pthsem"
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
  # yes, need arch for building this apu
  return 0
}


