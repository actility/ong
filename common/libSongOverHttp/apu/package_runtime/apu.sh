
projectName()
{
  echo "libSongOverHttp"
}

buildProject()
{
  cd libSongOverHttp && \
  make clean && \
  make

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/lib/ && \
  mkdir -p apu/control && \
  SOH_VERSION=`cat libSongOverHttp/Version` && \
  cp libSongOverHttp/lib/lib/libSongOverHttp.so \
     apu/data/lib/libSongOverHttp-$SOH_VERSION.so && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -f -s libSongOverHttp-$SOH_VERSION.so libSongOverHttp.so" >> apu/control/postinst 
}

projectArchDepend()
{
  # yes, need arch for building zigbee
  return 0
}

