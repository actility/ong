
buildProject()
{
  OPTIONS=
  if [ $1 == "centos6-x86" -o $1 == "centos6-x86_64" ]
  then
    OPTIONS="UNARY_TEST=on"
  fi

  cd AZAP && \
  make clean && \
  make $OPTIONS

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/AZAP/include/ && \
  mkdir -p apu/data/AZAP/lib/ && \
  mkdir -p apu/control && \
  cp AZAP/include/*.h apu/data/AZAP/include/ && \
  cp AZAP/lib/lib/*.a apu/data/AZAP/lib/
}

projectName()
{
  echo "AZAP-devel"
}

projectDescription()
{
  echo "AZAP - the Actility library to drive the ZigBee stack on TI CC2531 dongle stick"
}

projectArchDepend()
{
  # yes, need arch for building AZAP-devel
  return 0
}

