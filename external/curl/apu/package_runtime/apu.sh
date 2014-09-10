projectName()                                                                                       
{
  echo "curl"
}

buildProject()
{
  echo "compile ROOTACT: $ROOTACT"
  
  cd curl
  rm -rf curl-7.36.0
  unzip curl-7.36.0.zip
  cd curl-7.36.0
  if [ ! -f config.status ]; then
    _OPTIONS="--disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp "
    _OPTIONS=$_OPTIONS"--disable-dict --disable-telnet --disable-tftp "
    _OPTIONS=$_OPTIONS"--disable-pop3 --disable-imap --disable-smtp --disable-gopher "
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
  mkdir -p apu/control && \
  cp lib/libcurl.so.4.3.0 apu/data/lib/ && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -sf libcurl.so.4.3.0 libcurl.so" >> apu/control/postinst && \
  echo "ln -sf libcurl.so.4.3.0 libcurl.so.4" >> apu/control/postinst
}

projectDescription()
{
  echo "curl is a command line tool and a C library for transferring data with URL syntax, supporting HTTP and many other protocols"
}


