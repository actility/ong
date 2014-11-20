
applyPatch() 
{
  echo -n "Apply $1..."
  patch -p1 -i $1 || (cd - && echo "FAILED" && exit 1)
  echo "OK"
}

buildProject()
{
  cd tcpdump
  tar zxvf tcpdump-4.3.0.tar.gz
  cd tcpdump-4.3.0/

  export TARGET_DIR=$ROOTACT
  export PATH=$PATH:$ROOTACT/bin # this is required to locate pcap-config

  echo ">>> TARGET_DIR=$TARGET_DIR" 

  # apply patch
  sed -i 's:-L/lib:-L${TARGET_DIR}/lib:g' ./configure.in
  if [[ $1 != centos6-x86 ]]
  then
    applyPatch $ROOTACT/tcpdump/patches/configure.patch
    applyPatch $ROOTACT/tcpdump/patches/tcpdump_configure_no_-O2.patch
  fi

  autoreconf
  if [ ! -f Makefile ]; then
    if [ "$1" == "centos6-x86" -o "$1" == "centos6-x86_64" ]
    then
      echo ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
      ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
    else
      echo ac_cv_linux_vers=2 ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
      ac_cv_linux_vers=2 ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
    fi
  fi
  sed -i 's:/usr/lib:${TARGET_DIR}/lib:' ./Makefile
  sed -i 's:/usr/include:${TARGET_DIR}/include:' ./Makefile
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/control
 
  cp -r sbin/tcpdump apu/data/bin

  cp tcpdump/apu/$1/postinst apu/control/postinst
}

projectGroupId()
{
  echo "org.tcpdump"
}

projectName()
{
  echo "tcpdump"
}

projectVersion()
{
  echo "4.3.0"
}

projectApuRev()
{
  cat tcpdump/apu/revision
}

projectDescription()
{
  echo "tcpdump is a command-line packet analyzer"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

