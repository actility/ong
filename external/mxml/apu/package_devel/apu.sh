
buildProject()
{
  cd mxml
  rm -rf mxml-2.7/ $ROOTACT/lib/ $ROOTACT/include/
  tar zxvf mxml-2.7.tar.gz
  cd mxml-2.7/
  if [ ! -f Makefile ]; then
    echo ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
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


