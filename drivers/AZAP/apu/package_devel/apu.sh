
buildProject()
{
  cd AZAP
  if [[ $1 == centos6-x86 ]]
  then
    ./MAKE test
  else
    ./MAKE clean
    ./MAKE
  fi

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/AZAP/include/
  mkdir -p apu/data/AZAP/lib/
  mkdir -p apu/control
  
  cp AZAP/include/*.h apu/data/AZAP/include/
  cp AZAP/lib/*.a apu/data/AZAP/lib/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "AZAP-devel"
}

projectVersion()
{
  cat AZAP/Version
}

projectApuRev()
{
  cat AZAP/apu/revision
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

