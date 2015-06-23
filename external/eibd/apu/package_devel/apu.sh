
buildProject()
{
  CONFIGURE_OPTIONS="--enable-onlyeibd --enable-eibnetip --enable-eibnetiptunnel --enable-eibnetipserver --without-libstdc --with-pth=$ROOTACT --prefix $ROOTACT"
  case "$1" in 
    cov1)
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      ;;
    cov2)
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      ;;
    lpv3)
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      CONFIGURE_ADD_OPTIONS=$CONFIGURE_ADD_OPTIONS" LIBS=$ROOTACT/lib/libargp.a"
      ;;
    rpib)
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      ;;
    ntc6200)
      CONFIGURE_ADD_OPTIONS="--without-pth-test"
      ;;
    *)
      ;;
  esac

  cd eibd
  rm -rf bcusdk-0.0.5/
  tar zxvf bcusdk_0.0.5.tar.gz
  cd bcusdk-0.0.5/
  export LD_LIBRARY_PATH=$ROOTACT/lib
  
  #if [ ! -f Makefile ]; then
    echo ./configure $CONFIGURE_FLAGS --prefix $ROOTACT $CONFIGURE_OPTIONS $CONFIGURE_ADD_OPTIONS
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT $CONFIGURE_OPTIONS $CONFIGURE_ADD_OPTIONS
  #fi

  sed -i -e 's/SUBDIRS=.*/SUBDIRS=def c ./' eibd/client/Makefile.am && \
  make && \
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


