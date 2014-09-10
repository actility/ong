projectName()                                                                                       
{
  echo "libmicrohttpd-devel"
}

buildProject()
{
  echo "compile ROOTACT: $ROOTACT"

  __LIB_VERSION__=0.9.33 
 
  cd libmicrohttpd
  rm -rf libmicrohttpd-$__LIB_VERSION__
  tar zxvf libmicrohttpd-$__LIB_VERSION__.tar.gz
  cd libmicrohttpd-$__LIB_VERSION__
  if [ ! -f config.status ]; then
    _OPTIONS=
    echo "./configure $CONFIGURE_FLAGS --prefix $ROOTACT $_OPTIONS"
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT $_OPTIONS
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  echo "apu ROOTACT: $ROOTACT"
  rm -rf apu && \
  mkdir -p apu/data/lib && \
  mkdir -p apu/data/include && \
  mkdir -p apu/control && \
  cp lib/libmicrohttpd.a apu/data/lib/ && \
  cp lib/libmicrohttpd.la apu/data/lib/ && \
  cp lib/libmicrohttpd.so.10.22.0 apu/data/lib/ && \
  cp -r include/* apu/data/include/ && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -sf libmicrohttpd.so.10.22.0 libmicrohttpd.so" >> apu/control/postinst && \
  echo "ln -sf libmicrohttpd.so.10.22.0 libmicrohttpd.so.10" >> apu/control/postinst
}

projectDescription()
{
  echo "libmicrohttpd is a C library for embedding an HTTP(S) server into C applications"
}


