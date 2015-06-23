
buildProject()
{
  cd libcoap && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r libcoap/ apu/data/
}

projectName()
{
  echo "libcoap-src"
}

projectDescription()
{
  echo "Source files of CoAP library"
}

projectArchDepend()
{
  # no, no need of arch for building src apu
  return 1
}

