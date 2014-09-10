
buildProject()
{
  cd libcoap
  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/libcoap/
  mkdir -p apu/control
  
  cp libcoap/*.h apu/data/libcoap/
  cp libcoap/*.a apu/data/libcoap/
  cp libcoap/Makefile apu/data/libcoap/
}

projectGroupId()
{
  echo "org.tzi"
}

projectName()
{
  echo "libcoap-devel"
}

projectVersion()
{
  cat libcoap/Version
}

projectApuRev()
{
  cat libcoap/apu/revision
}

projectDescription()
{
  echo "Headers and compiled binary for CoAP library"
}

projectArchDepend()
{
  # yes, need arch for building AZAP-devel
  return 0
}

