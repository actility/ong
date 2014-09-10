
buildProject()
{
  cd libcoap
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r libcoap/ apu/data/
}

projectGroupId()
{
  echo "org.tzi"
}

projectName()
{
  echo "libcoap-src"
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
  echo "Source files of CoAP library"
}

projectArchDepend()
{
  # no, no need of arch for building src apu
  return 1
}

