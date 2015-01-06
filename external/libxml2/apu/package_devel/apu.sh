
buildProject()
{
  echo "compile ROOTACT: $ROOTACT"
  
  cd libxml2
  tar zxvf libxml2-2.7.8.tar.gz
  cd libxml2-2.7.8
  if [ ! -f Makefile ]; then
    echo ./configure $CONFIGURE_FLAGS --prefix $ROOTACT --without-python --without-legacy
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT --without-python --without-legacy
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
  XML_VERSION=$(projectVersion) && \
  cp lib/libxml2.so.$XML_VERSION apu/data/lib/ && \
  cp -r include/* apu/data/include/ && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -fs libxml2.so.$XML_VERSION libxml2.so" >> apu/control/postinst && \
  echo "ln -fs libxml2.so.$XML_VERSION libxml2.so.2" >> apu/control/postinst
}

projectNameSuffix()
{
  echo "devel"
}

projectDescription()
{
  echo "Headers from libxml2 (library to parse and encode XML documents)"
}

