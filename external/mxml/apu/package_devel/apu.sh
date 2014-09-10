
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

  cd mxml
  rm -rf mxml-2.7/ $ROOTACT/lib/ $ROOTACT/include/
  tar zxvf mxml-2.7.tar.gz
  cd mxml-2.7/
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
  _ret=$?

  if [ -f $ROOTACT/mxml/mxml-2.7/libmxml.a ]; then
    #make install
    mkdir -p $ROOTACT/include && \
    mkdir -p $ROOTACT/lib && \
    install -m644 $ROOTACT/mxml/mxml-2.7/mxml.h $ROOTACT/include && \
    install -m644 $ROOTACT/mxml/mxml-2.7/libmxml.a $ROOTACT/lib && \
    install -m775 $ROOTACT/mxml/mxml-2.7/libmxml.so.1.5 $ROOTACT/lib
    _ret=$? 
  fi

  return $_ret
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r include apu/data/ && \
  cp -r lib apu/data/ && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -s libmxml.so.1.5 libmxml.so" >> apu/control/postinst && \
  echo "ln -s libmxml.so.1.5 libmxml.so.1" >> apu/control/postinst
}

projectGroupId()
{
  echo "org.msweet"
}

projectName()
{
  echo "mxml-devel"
}

projectVersion()
{
  echo "2.7"
}

projectApuRev()
{
  cat mxml/apu/revision
}

projectDescription()
{
  echo "Tiny library for XML parsing."
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}


