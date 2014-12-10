
buildProject()
{
  cd pthsem
  tar zxvf pthsem_2.0.8.tar.gz
  cd pthsem-2.0.8/
  if [ ! -f Makefile ]; then
    echo ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
 
  PCAP_VERSION=$(projectVersion)

  cp -r include apu/data/
  cp -r lib apu/data/ 
  cp -r bin apu/data/

  cp pthsem/apu/$1/postinst apu/control/postinst
}

projectGroupId()
{
  echo "org.gnu"
}

projectName()
{
  echo "pthsem-devel"
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
  # yes, need arch for building devel apu
  return 0
}


