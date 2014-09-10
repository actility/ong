
projectName()
{
  echo "libmodbus-devel"
}

buildProject()
{
  cd libmodbus
  rm -rf libmodbus-3.0.5
  tar zxvf libmodbus-3.0.5.tar.gz
  cd libmodbus-3.0.5
  if [ ! -f Makefile ]; then
    _OPTIONS="--enable-shared=no --enable-static=yes"
    echo "./configure $CONFIGURE_FLAGS --prefix $ROOTACT $_OPTIONS"
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT $_OPTIONS
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r include apu/data/ && \
  cp -r lib apu/data/ && \
  cp -r share apu/data/
}

projectDescription()
{
  echo "Library to send/receive data over ModBus protocol."
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}


