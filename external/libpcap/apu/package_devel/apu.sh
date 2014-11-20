
buildProject()
{
  cd libpcap
  tar zxvf libpcap-1.3.0.tar.gz
  cd libpcap-1.3.0/
  if [ ! -f Makefile ]; then
    echo ./configure --with-pcap=linux $CONFIGURE_FLAGS --prefix $ROOTACT
    ./configure --with-pcap=linux $CONFIGURE_FLAGS --prefix $ROOTACT
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

  cp libpcap/apu/$1/postinst apu/control/postinst 
}

projectGroupId()
{
  echo "org.tcpdump"
}

projectName()
{
  echo "libpcap-devel"
}

projectVersion()
{
  echo "1.3.0"
}

projectApuRev()
{
  cat libpcap/apu/revision
}

projectDescription()
{
  echo "Library for user-level packet capture."
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}


