
buildProject()
{
  case "$1" in 
    cov1)
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    cov2)
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    lpv3)
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    ntc6200)
      CONFIGURE_ADD_OPTIONS="--with-mctx-dsp=ssjlj --with-mctx-stk=sas --with-mctx-mth=sjlj"
      ;;
    *)
      ;;
  esac

  cd pthsem
  tar zxvf pthsem_2.0.8.tar.gz
  cd pthsem-2.0.8/
  if [ ! -f Makefile ]; then
    echo ./configure $CONFIGURE_FLAGS --prefix $ROOTACT $CONFIGURE_ADD_OPTIONS
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT $CONFIGURE_ADD_OPTIONS
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


