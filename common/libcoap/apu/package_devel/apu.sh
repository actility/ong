
buildProject()
{
  cd libcoap && \
  make clean && \
  make

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/libcoap/ && \
  mkdir -p apu/control && \
  cp libcoap/*.h apu/data/libcoap/ && \
  cp libcoap/*.a apu/data/libcoap/ && \
  cp libcoap/Makefile apu/data/libcoap/
}

projectName()
{
  echo "libcoap-devel"
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

