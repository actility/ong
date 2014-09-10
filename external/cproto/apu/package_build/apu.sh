projectName()                                                                                       
{
  echo "cproto-build"
}

buildProject()
{
  echo "compile ROOTACT: $ROOTACT"
  
  cd cproto
  rm -rf cproto-4.6
  tar zxvf cproto-4.6.tar.gz
  cd cproto-4.6
  if [ ! -f config.status ]; then
    echo "./configure $CONFIGURE_FLAGS --prefix $ROOTACT"
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  echo "apu ROOTACT: $ROOTACT"
  rm -rf apu && \
  mkdir -p apu/data/bin && \
  mkdir -p apu/control && \
  cp bin/cproto apu/data/bin/ && \
  echo "cd \$ROOTACT/bin" > apu/control/postinst && \
  echo "chmod +x cproto" >> apu/control/postinst
}

projectDescription()
{
  echo "Cproto is a program that generates function prototypes and variable declarations from C source code."
}


